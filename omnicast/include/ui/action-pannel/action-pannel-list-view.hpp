#pragma once
#include "ui/action-pannel/action-pannel-view.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/omni-list.hpp"

class ActionPannelListView : public ActionPannelView {
protected:
  OmniList *_list;
  std::vector<std::shared_ptr<AbstractAction>> _actions;
  std::vector<ActionItem> _items;

  bool eventFilter(QObject *sender, QEvent *event) override;

public:
  ActionPannelListView();

  std::vector<std::shared_ptr<AbstractAction>> actions() const override;
  void onItemActivated(const OmniList::AbstractVirtualItem &item);
  void renderActionPannelModel(std::vector<ActionItem> actions);
};
