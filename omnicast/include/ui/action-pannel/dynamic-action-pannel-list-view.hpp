#include "ui/action-pannel/action-pannel-list-view.hpp"
#include "ui/action-pannel/action-section.hpp"

class DynamicActionPannelListView : public ActionPannelListView {
  AbstractActionSection *_section;

public:
  void onSearchChanged(const QString &query) override {
    qDebug() << "Generate actions from search " << query;
    renderActionPannelModel(_section->generateItems(query));
  }

  DynamicActionPannelListView(AbstractActionSection *section) : _section(section) {}
};
