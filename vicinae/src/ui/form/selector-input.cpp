#include "ui/form/selector-input.hpp"
#include "common.hpp"
#include "../image/url.hpp"
#include "theme.hpp"
#include "ui/focus-notifier.hpp"
#include "ui/image/image.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qwidget.h>

void SelectorInput::updateModel() {
  m_list->updateModel([&]() {
    for (const auto &section : m_sections) {
      auto &sec = m_list->addSection(section.title);

      for (const auto &item : section.items) {
        sec.addItem(item);
      }
    }
  });
}

void SelectorInput::addSection(const QString &title,
                               const std::vector<std::shared_ptr<AbstractItem>> &items) {
  m_sections.emplace_back(DropdownSection(title, items));
}

void SelectorInput::resetModel() { m_sections.clear(); }

void SelectorInput::listHeightChanged(int height) {

  if (height > 0) {
    m_content->setCurrentIndex(0);
    popover->setFixedHeight(std::min(POPOVER_HEIGHT, m_searchField->sizeHint().height() + 1 + height));
    return;
  }

  m_content->setCurrentIndex(1);
  popover->setFixedHeight(
      std::min(POPOVER_HEIGHT, m_searchField->sizeHint().height() + 1 + m_content->sizeHint().height()));
}

bool SelectorInput::eventFilter(QObject *obj, QEvent *event) {
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

QJsonValue SelectorInput::asJsonValue() const {
  return _currentSelection ? _currentSelection->id() : QJsonValue();
}

void SelectorInput::setIsLoading(bool value) { m_loadingBar->setStarted(value); }

void SelectorInput::setValueAsJson(const QJsonValue &value) { setValue(value.toString()); }

FocusNotifier *SelectorInput::focusNotifier() const { return m_focusNotifier; }

SelectorInput::SelectorInput(QWidget *parent)
    : JsonFormItemWidget(parent), m_list(new OmniList), inputField(new BaseInput),
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

  // auto listContainerWidget = new QWidget;
  // auto listContainerLayout = new QVBoxLayout;

  // listContainerLayout->setContentsMargins(0, 0, 0, 0);
  // listContainerLayout->addWidget(m_list);
  // listContainerWidget->setLayout(listContainerLayout);

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

  connect(m_searchField, &QLineEdit::textChanged, this, &SelectorInput::handleTextChanged);
  connect(m_list, &OmniList::itemActivated, this, &SelectorInput::itemActivated);
  connect(m_list, &OmniList::itemUpdated, this, &SelectorInput::itemUpdated);
  connect(m_list, &OmniList::virtualHeightChanged, this, &SelectorInput::listHeightChanged);
  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() {
    style()->unpolish(this);
    style()->polish(this);
  });

  setLayout(layout);
}

void SelectorInput::itemActivated(const OmniList::AbstractVirtualItem &vitem) {
  setValue(vitem.id());
  m_searchField->clear();
  popover->close();
  emit selectionChanged(static_cast<const AbstractItem &>(vitem));
}

QString SelectorInput::searchText() { return m_searchField->text(); }

void SelectorInput::updateItem(const QString &id, const UpdateItemCallback &cb) {
  m_list->updateItem(id,
                     [&cb](OmniList::AbstractVirtualItem *item) { cb(static_cast<AbstractItem *>(item)); });
  if (_currentSelection && _currentSelection->generateId() == id) { setValue(id); }
}

const SelectorInput::AbstractItem *SelectorInput::value() const { return _currentSelection.get(); }

bool SelectorInput::setValue(const QString &id) {
  auto selectedItem = m_list->setSelected(id);

  if (!selectedItem) {
    qWarning() << "selectValue: no item with ID:" << id;
    return false;
  }

  auto item = static_cast<const AbstractItem *>(selectedItem);
  auto icon = item->icon();

  _currentSelection.reset(item->clone());

  inputField->setText(item->displayName());
  inputField->update();

  if (icon) {
    selectionIcon->show();
    selectionIcon->setUrl(*icon);
  }

  inputField->recalculate();

  return true;
}

void SelectorInput::setEnableDefaultFilter(bool value) {
  if (value == m_defaultFilterEnabled) return;

  m_defaultFilterEnabled = value;
}

void SelectorInput::handleTextChanged(const QString &text) {

  // TODO: improve this, probably support sorted sections as well
  if (m_defaultFilterEnabled) {
    m_list->updateModel([&]() {
      for (const auto &section : m_sections) {
        auto &listSection = m_list->addSection(section.title);
        if (!text.isEmpty()) {
          for (const auto &scored : section.search(text)) {
            listSection.addItem(scored.item);
          }
        } else {
          for (const auto &item : section.items) {
            listSection.addItem(item);
          }
        }
      }
    });
  }

  emit textChanged(text);
}

void SelectorInput::itemUpdated(const OmniList::AbstractVirtualItem &item) {
  if (_currentSelection && _currentSelection->generateId() == item.generateId()) setValue(item.generateId());
}

void SelectorInput::showPopover() {
  const QPoint globalPos = inputField->mapToGlobal(QPoint(0, inputField->height() + 10));

  if (_currentSelection) {
    m_list->setSelected(_currentSelection->generateId());
  } else {
    m_list->selectFirst();
  }

  popover->move(globalPos);
  popover->resize(inputField->width(), POPOVER_HEIGHT);
  popover->show();
}

SelectorInput::~SelectorInput() {}

void SelectorInput::clear() {
  inputField->clear();
  _currentSelection.reset();
}
