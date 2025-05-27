#include "ui/action-pannel/action-list-item.hpp"

void ActionListItem::setup(ActionListWidget *widget) const { widget->setAction(action); }

QString ActionListItem::id() const { return action->id(); }

OmniListItemWidget *ActionListItem::createWidget() const {
  auto widget = new ActionListWidget;

  setup(widget);

  return widget;
}

int ActionListItem::calculateHeight(int width) const {
  static ActionListWidget ruler;

  return ruler.sizeHint().height();
}

bool ActionListItem::recyclable() const { return true; }

void ActionListItem::recycle(QWidget *base) const { setup(static_cast<ActionListWidget *>(base)); }

ActionListItem::ActionListItem(AbstractAction *action) : action(action) {}
