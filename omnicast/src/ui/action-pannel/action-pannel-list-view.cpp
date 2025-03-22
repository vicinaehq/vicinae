#include "ui/action-pannel/action-pannel-list-view.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action-list-item.hpp"
#include "ui/action-pannel/action-section.hpp"
#include "ui/action-pannel/action.hpp"

bool ActionPannelListView::eventFilter(QObject *sender, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);

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

  return false;
}

void ActionPannelListView::onItemActivated(const OmniList::AbstractVirtualItem &item) {
  auto &listItem = static_cast<const ActionListItem &>(item);

  emit actionActivated(listItem.action);
}

void ActionPannelListView::renderActionPannelModel(const std::vector<ActionItem> &actions) {
  std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> items;

  _actions.clear();

  for (auto &actionLike : actions) {
    if (auto ptr = std::get_if<AbstractAction *>(&actionLike)) {
      auto action = *ptr;

      _actions.push_back(action);
      items.push_back(std::make_unique<ActionListItem>(action));
    }

    if (auto ptr = std::get_if<AbstractActionSection *>(&actionLike)) {
      _actions.push_back(*ptr);
      items.push_back(std::make_unique<ActionListItem>(*ptr));
    }

    else if (auto ptr = std::get_if<ActionLabel>(&actionLike)) {
      items.push_back(std::make_unique<OmniList::VirtualSection>(ptr->label()));
    }
  }

  _list->updateFromList(items, OmniList::SelectionPolicy::SelectFirst);
}

std::vector<AbstractAction *> ActionPannelListView::actions() const { return _actions; }

ActionPannelListView::ActionPannelListView() : _list(new OmniList()) {
  auto layout = new QVBoxLayout;

  layout->addWidget(_list);

  setLayout(layout);
  installEventFilter(this);
  connect(_list, &OmniList::itemActivated, this, &ActionPannelListView::onItemActivated);
}
