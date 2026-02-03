#include "ui/action-pannel/action-list-item.hpp"

void ActionListItem::setup(ActionListWidget *widget) const { widget->setAction(action); }

QString ActionListItem::generateId() const { return action->id(); }

OmniListItemWidget *ActionListItem::createWidget() const {
  auto widget = new ActionListWidget;
  setup(widget);
  return widget;
}

bool ActionListItem::recyclable() const { return true; }

void ActionListItem::recycle(QWidget *base) const { setup(static_cast<ActionListWidget *>(base)); }

int ActionListItem::calculateHeight(int width) const { return 40; }

size_t ActionListItem::recyclingId() const { return typeid(ActionListItem).hash_code(); }

ActionListItem::ActionListItem(AbstractAction *action) : action(action) {}
