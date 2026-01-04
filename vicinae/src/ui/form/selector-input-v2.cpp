#include "ui/form/selector-input-v2.hpp"
#include "common.hpp"
#include "../image/url.hpp"
#include <QStyle>
#include "theme.hpp"
#include "ui/focus-notifier.hpp"
#include "ui/image/image.hpp"
#include "ui/vlist/common/dropdown-model.hpp"
#include "ui/vlist/vlist.hpp"
#include <qboxlayout.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qwidget.h>

void SelectorInputV2::listHeightChanged(int height) {

  if (height > 0) {
    m_content->setCurrentIndex(0);
    popover->setFixedHeight(std::min(POPOVER_HEIGHT, m_searchField->sizeHint().height() + 1 + height));
    return;
  }

  m_content->setCurrentIndex(1);
  popover->setFixedHeight(
      std::min(POPOVER_HEIGHT, m_searchField->sizeHint().height() + 1 + m_content->sizeHint().height()));
}

bool SelectorInputV2::eventFilter(QObject *obj, QEvent *event) {
  if (obj == popover) {
    if (event->type() == QEvent::Close) {
      collapseIcon->setUrl(ImageURL::builtin("chevron-down"));
      m_searchField->clear();
    } else if (event->type() == QEvent::Show) {
      collapseIcon->setUrl(ImageURL::builtin("chevron-up"));
      m_searchField->setFocus();
    }
  }

  if (obj == m_searchField) {
    if (event->type() == QEvent::KeyPress) {
      auto key = static_cast<QKeyEvent *>(event)->key();

      if (key == Qt::Key_Up || key == Qt::Key_Down || key == Qt::Key_Return || key == Qt::Key_Enter) {
        QApplication::sendEvent(m_list, event);
        return true;
      }
    }
  }

  if (obj == inputField->input()) {
    if (event->type() == QEvent::KeyPress) {
      auto kv = static_cast<QKeyEvent *>(event);

      if (kv->modifiers() == Qt::Modifiers{} && kv->key() == Qt::Key_Return) {
        showPopover();
        return true;
      }
    }

    if (event->type() == QEvent::MouseButtonPress) {
      showPopover();
      return true;
    }
  }

  return QWidget::eventFilter(obj, event);
}

QJsonValue SelectorInputV2::asJsonValue() const {
  return value().transform([](auto &&data) -> QJsonValue { return data.id; }).value_or(QJsonValue::Null);
}

void SelectorInputV2::setIsLoading(bool value) { m_loadingBar->setStarted(value); }

void SelectorInputV2::setValueAsJson(const QJsonValue &value) { setValue(value.toString()); }

FocusNotifier *SelectorInputV2::focusNotifier() const { return m_focusNotifier; }

void SelectorInputV2::setModel(vicinae::ui::DropdownModel *model) {
  m_list->setModel(model);
  m_model = model;
}

SelectorInputV2::SelectorInputV2(QWidget *parent)
    : JsonFormItemWidget(parent), m_list(new vicinae::ui::VListWidget), inputField(new BaseInput),
      m_searchField(new QLineEdit()), popover(new Popover(this)), collapseIcon(new ImageWidget),
      selectionIcon(new ImageWidget) {
  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);

  m_loadingBar->setPositionStep(5);

  setFocusProxy(inputField);

  connect(inputField->focusNotifier(), &FocusNotifier::focusChanged, this, [this](bool value) {
    // opening the selection menu is NOT a focus change
    if (!popover->isVisible()) {
      if (m_focused != value) {
        emit m_focusNotifier->focusChanged(value);
        m_focused = value;
      }
    }
  });

  popover->setProperty("class", "popover");

  // Main input field
  inputField->setPlaceholderText("Select an item...");
  inputField->setReadOnly(true); // Read-only to behave like a combo box
  collapseIcon->setUrl(ImageURL::builtin("chevron-down"));
  inputField->setLeftAccessory(selectionIcon);
  inputField->setRightAccessory(collapseIcon);

  layout->addWidget(inputField);

  // Create the popover
  popover->setWindowFlags(Qt::Popup);
  auto *popoverLayout = new QVBoxLayout(popover);
  popoverLayout->setContentsMargins(0, 0, 0, 0);
  popoverLayout->setSpacing(0);

  m_searchField = new QLineEdit(popover);
  m_searchField->setContentsMargins(15, 15, 15, 15);
  m_searchField->setPlaceholderText("Search...");
  popoverLayout->addWidget(m_searchField);

  popoverLayout->addWidget(m_loadingBar);

  inputField->input()->installEventFilter(this);
  m_searchField->installEventFilter(this);
  popover->installEventFilter(this);

  auto emptyLayout = new QVBoxLayout;
  auto emptyTypography = new TypographyWidget();

  emptyTypography->setContentsMargins(10, 10, 10, 10);
  emptyTypography->setText("No results");
  emptyTypography->setColor(SemanticColor::Foreground);
  emptyTypography->setAlignment(Qt::AlignCenter);
  emptyLayout->addWidget(emptyTypography);
  m_emptyView->setLayout(emptyLayout);

  m_content->addWidget(m_list);
  m_content->addWidget(m_emptyView);
  m_content->setCurrentIndex(0);

  popoverLayout->addWidget(m_content);

  selectionIcon->hide();

  connect(m_searchField, &QLineEdit::textChanged, this, &SelectorInputV2::handleTextChanged);
  // connect(m_list, &OmniList::itemActivated, this, &SelectorInputV2::itemActivated);
  // connect(m_list, &OmniList::virtualHeightChanged, this, &SelectorInputV2::listHeightChanged);

  connect(m_list, &vicinae::ui::VListWidget::itemActivated, this, [&](vicinae::ui::VListModel::Index idx) {
    m_searchField->clear();
    popover->close();
    emit selectionChanged(m_model->fromIndex(idx).value());
  });

  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() {
    style()->unpolish(this);
    style()->polish(this);
  });

  setLayout(layout);
}

QString SelectorInputV2::searchText() { return m_searchField->text(); }

std::optional<vicinae::ui::DropdownItem> SelectorInputV2::value() const {
  if (!m_model) return {};
  return m_list->currentSelection().and_then([&](auto index) { return m_model->fromIndex(index); });
}

bool SelectorInputV2::setValue(const QString &id) {
  auto value = m_model->indexForId(id);

  m_list->setSelected(m_model->indexForId(id));

  if (!value) {
    qWarning() << "selectValue: no item with ID:" << id;
    return false;
  }

  auto data = m_model->fromIndex(value.value());

  inputField->setText(data->id);
  inputField->update();

  if (data->icon) {
    selectionIcon->show();
    selectionIcon->setUrl(data->icon.value());
  }

  inputField->recalculate();

  return true;
}

void SelectorInputV2::handleTextChanged(const QString &text) { emit textChanged(text); }

void SelectorInputV2::showPopover() {
  const QPoint globalPos = inputField->mapToGlobal(QPoint(0, inputField->height() + 10));

  popover->move(globalPos);
  popover->resize(inputField->width(), POPOVER_HEIGHT);
  popover->show();
}
