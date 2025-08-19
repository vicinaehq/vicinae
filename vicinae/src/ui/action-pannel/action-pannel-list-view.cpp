#include "ui/action-pannel/action-pannel-list-view.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action-list-item.hpp"
#include "ui/action-pannel/action.hpp"

bool ActionPannelListView::eventFilter(QObject *sender, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);

    if (keyEvent->modifiers() == Qt::ControlModifier) {
      switch (keyEvent->key()) {
      case Qt::Key_J:
        return _list->selectDown();
      case Qt::Key_K:
        return _list->selectUp();
      case Qt::Key_H:
        context()->navigation->popCurrentView();
        return true;
      case Qt::Key_L:
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
