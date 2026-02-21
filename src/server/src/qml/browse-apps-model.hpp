#pragma once
#include "fuzzy-list-model.hpp"
#include "services/app-service/abstract-app-db.hpp"

using AppPtr = std::shared_ptr<AbstractApplication>;

template <> struct fuzzy::FuzzySearchable<AppPtr> {
  static int score(const AppPtr &app, std::string_view query) {
    auto name = app->displayName().toStdString();
    auto desc = app->description().toStdString();
    int best = fuzzy::scoreWeighted({{name, 1.0}, {desc, 0.5}}, query);
    for (const auto &kw : app->keywords()) {
      auto s = kw.toStdString();
      int kwScore = fuzzy::scoreWeighted({{s, 0.7}}, query);
      if (kwScore > best) best = kwScore;
    }
    return best;
  }
};

class BrowseAppsModel : public FuzzyListModel<AppPtr> {
  Q_OBJECT

public:
  using FuzzyListModel::FuzzyListModel;

protected:
  QString displayTitle(const AppPtr &app) const override;
  QString displaySubtitle(const AppPtr &app) const override;
  QString displayIconSource(const AppPtr &app) const override;
  QVariant displayAccessory(const AppPtr &app) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const AppPtr &app) const override;
  QString sectionLabel() const override;
};
