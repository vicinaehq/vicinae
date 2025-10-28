#include "ui/action-pannel/action-pannel-list-view.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action-list-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "services/keybinding/keybinding-service.hpp"
#include "services/config/config-service.hpp"
#include "service-registry.hpp"

bool ActionPannelListView::eventFilter(QObject *sender, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    auto config = ServiceRegistry::instance()->config();
    const QString &keybinding = config->value().keybinding;

    if (keyEvent->modifiers() == Qt::ControlModifier) {
      if (KeyBindingService::isDownKey(keyEvent, keybinding)) {
        return _list->selectDown();
      }
      if (KeyBindingService::isUpKey(keyEvent, keybinding)) {
        return _list->selectUp();
      }
      if (KeyBindingService::isLeftKey(keyEvent, keybinding)) {
        context()->navigation->popCurrentView();
        return true;
      }
      if (KeyBindingService::isRightKey(keyEvent, keybinding)) {
        _list->activateCurrentSelection();
        return true;
      }
    }

    if (keyEvent->modifiers().toInt() == 0) {
      switch (keyEvent->key()) {
      case Qt::Key_Up:
        return _list->selectUp();
      case Qt::Key_Down:
        return _list->selectDown();
      case Qt::Key_Home:
        return _list->selectHome();
      case Qt::Key_End:
        return _list->selectEnd();
      case Qt::Key_Return:
        _list->activateCurrentSelection();
        return true;
      }
    }
  }

  return false;
}

void ActionPannelListView::onItemActivated(const OmniList::AbstractVirtualItem &item) {
  auto &listItem = static_cast<const ActionListItem &>(item);

  emit actionActivated(listItem.action);
}

void ActionPannelListView::renderActionPannelModel(std::vector<ActionItem> actions) {
  std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> items;

  _actions.clear();
  _items.clear();

  for (auto &actionLike : actions) {
    if (auto ptr = std::get_if<std::shared_ptr<AbstractAction>>(&actionLike)) {
      _actions.push_back(*ptr);
      items.push_back(std::make_unique<ActionListItem>(ptr->get()));
    }

    else if (auto ptr = std::get_if<ActionLabel>(&actionLike)) {
      items.push_back(std::make_unique<OmniList::VirtualSection>(ptr->label(), false));
    }
  }

  _items = actions;
  _list->updateFromList(items, OmniList::SelectionPolicy::SelectFirst);
}

std::vector<std::shared_ptr<AbstractAction>> ActionPannelListView::actions() const { return _actions; }

ActionPannelListView::ActionPannelListView() : _list(new OmniList()) {
  auto layout = new QVBoxLayout;

  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(_list);

  setLayout(layout);
  installEventFilter(this);
  connect(_list, &OmniList::itemActivated, this, &ActionPannelListView::onItemActivated);
  connect(_list, &OmniList::virtualHeightChanged, this, [this](int height) {
    qDebug() << "virtual height changed" << height;
    _list->setFixedHeight(height);
    updateGeometry();
  });
}
