#include "ui/form/selector-input.hpp"
#include "common.hpp"
#include <memory>
#include <qjsonvalue.h>

bool SelectorInput::eventFilter(QObject *obj, QEvent *event) {
  if (obj == popover) {
    if (event->type() == QEvent::Close) {
      collapseIcon->setUrl(BuiltinOmniIconUrl("chevron-down"));
      searchField->clear();
    } else if (event->type() == QEvent::Show) {
      collapseIcon->setUrl(BuiltinOmniIconUrl("chevron-up"));
      searchField->setFocus();
    }
  }

  if (obj == searchField) {
    if (event->type() == QEvent::KeyPress) {
      auto key = static_cast<QKeyEvent *>(event)->key();

      if (key == Qt::Key_Up || key == Qt::Key_Down || key == Qt::Key_Return || key == Qt::Key_Enter) {
        QApplication::sendEvent(m_list, event);
        return true;
      }
    }
  }

  if (obj == inputField) {
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

  return false;
}

QJsonValue SelectorInput::asJsonValue() const {
  return _currentSelection ? _currentSelection->id() : QJsonValue();
}

void SelectorInput::setValueAsJson(const QJsonValue &value) { setValue(value.toString()); }

SelectorInput::SelectorInput(const QString &name)
    : m_list(new OmniList), inputField(new BaseInput), searchField(new QLineEdit()),
      popover(new Popover(this)), collapseIcon(new OmniIcon), selectionIcon(new OmniIcon) {
  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);

  popover->setProperty("class", "popover");

  // Main input field
  inputField->setPlaceholderText("Select an item...");
  inputField->setReadOnly(true); // Read-only to behave like a combo box
  collapseIcon->setUrl(BuiltinOmniIconUrl("chevron-down"));
  inputField->setLeftAccessory(selectionIcon);
  inputField->setRightAccessory(collapseIcon);
  layout->addWidget(inputField);

  // Create the popover
  popover->setWindowFlags(Qt::Popup);
  auto *popoverLayout = new QVBoxLayout(popover);
  popoverLayout->setContentsMargins(1, 1, 1, 1);
  popoverLayout->setSpacing(0);

  searchField = new QLineEdit(popover);
  searchField->setContentsMargins(15, 15, 15, 15);
  searchField->setPlaceholderText("Search...");
  popoverLayout->addWidget(searchField);

  popoverLayout->addWidget(new HDivider);

  inputField->installEventFilter(this);
  searchField->installEventFilter(this);
  popover->installEventFilter(this);

  auto listContainerWidget = new QWidget;
  auto listContainerLayout = new QVBoxLayout;

  listContainerLayout->setContentsMargins(0, 0, 0, 0);
  listContainerLayout->addWidget(m_list);
  listContainerWidget->setLayout(listContainerLayout);

  popoverLayout->addWidget(listContainerWidget);

  connect(searchField, &QLineEdit::textChanged, this, &SelectorInput::handleTextChanged);
  connect(m_list, &OmniList::itemActivated, this, &SelectorInput::itemActivated);
  connect(m_list, &OmniList::itemUpdated, this, &SelectorInput::itemUpdated);

  setLayout(layout);
}

void SelectorInput::itemActivated(const OmniList::AbstractVirtualItem &vitem) {
  setValue(vitem.id());
  searchField->clear();
  popover->close();
  emit selectionChanged(static_cast<const AbstractItem &>(vitem));
}

QString SelectorInput::searchText() { return searchField->text(); }

void SelectorInput::beginUpdate() { m_list->beginUpdate(); }

void SelectorInput::commitUpdate() { m_list->commitUpdate(); }

void SelectorInput::updateItem(const QString &id, const UpdateItemCallback &cb) {
  m_list->updateItem(id,
                     [&cb](OmniList::AbstractVirtualItem *item) { cb(static_cast<AbstractItem *>(item)); });
}

void SelectorInput::addSection(const QString &name) { m_list->addSection(name); }

void SelectorInput::addItem(std::unique_ptr<AbstractItem> item) { m_list->addItem(std::move(item)); }

const SelectorInput::AbstractItem *SelectorInput::value() const { return _currentSelection.get(); }

void SelectorInput::setValue(const QString &id) {
  auto selectedItem = m_list->setSelected(id);

  if (!selectedItem) {
    qDebug() << "selectValue: no item with ID:" << id;
    return;
  }

  auto item = static_cast<const AbstractItem *>(selectedItem);

  _currentSelection.reset(item->clone());
  selectionIcon->setUrl(item->icon());
  inputField->setText(item->displayName());
}

void SelectorInput::handleTextChanged(const QString &text) {
  m_list->setFilter(std::make_unique<ItemFilter>(text));
  emit textChanged(text);
}

void SelectorInput::itemUpdated(const OmniList::AbstractVirtualItem &item) {
  if (_currentSelection && _currentSelection->id() == item.id()) setValue(item.id());
}

void SelectorInput::showPopover() {
  const QPoint globalPos = inputField->mapToGlobal(QPoint(0, inputField->height() + 10));

  if (_currentSelection) {
    m_list->setSelected(_currentSelection->id());
  } else {
    m_list->selectFirst();
  }

  popover->move(globalPos);
  popover->resize(inputField->width(), POPOVER_HEIGHT);
  popover->show();
}

SelectorInput::~SelectorInput() {
  qDebug() << "~FormDropdown2";
  popover->deleteLater();
}

void SelectorInput::clear() {
  inputField->clear();
  m_list->clear();
  _currentSelection.reset();
}
