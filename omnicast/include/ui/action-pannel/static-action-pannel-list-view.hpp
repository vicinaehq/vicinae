#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action-pannel-list-view.hpp"

class StaticActionPannelListView : public ActionPannelListView {
public:
  StaticActionPannelListView(const std::vector<ActionItem> &items) { renderActionPannelModel(items); }
};
