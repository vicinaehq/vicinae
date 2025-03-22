#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action-list-item.hpp"
#include "ui/action-pannel/action-pannel-list-view.hpp"
#include "ui/omni-list.hpp"
#include <memory>
#include <qnamespace.h>

class StaticActionPannelListView : public ActionPannelListView {
  class StaticActionFilter : public OmniList::AbstractItemFilter {
    QString _query;

    bool matches(const OmniList::AbstractVirtualItem &item) override {
      auto &actionItem = static_cast<const ActionListItem &>(item);

      return actionItem.action->title.contains(_query, Qt::CaseInsensitive);
    }

  public:
    StaticActionFilter(const QString &query) : _query(query) {}
  };

public:
  StaticActionPannelListView(const std::vector<ActionItem> &items) { renderActionPannelModel(items); }

  void onSearchChanged(const QString &query) override {
    _list->setFilter(std::make_unique<StaticActionFilter>(query));
  }
};
