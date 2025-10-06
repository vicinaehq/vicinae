#pragma once
#include "ui/omni-list/omni-list.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/action-pannel/action-list-widget.hpp"

class ActionListItem : public OmniList::AbstractVirtualItem {
public:
  AbstractAction *action;

  QString generateId() const override;
  OmniListItemWidget *createWidget() const override;
  bool recyclable() const override;
  void setup(ActionListWidget *widget) const;
  void recycle(QWidget *base) const override;
  size_t recyclingId() const override;
  int calculateHeight(int width) const override;

public:
  ActionListItem(AbstractAction *action);
};
