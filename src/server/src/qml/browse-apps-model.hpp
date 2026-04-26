#pragma once
#include "fuzzy-list-model.hpp"
#include "services/app-service/abstract-app-db.hpp"

using AppPtr = std::shared_ptr<AbstractApplication>;

template <> struct fuzzy::FuzzySearchable<AppPtr> {
  static int score(const AppPtr &app, std::string_view query) {
    auto name = app->displayName().toStdString();
    auto desc = app->description().toStdString();

    std::vector<std::string> keywords;
    keywords.reserve(app->keywords().size());
    for (const auto &kw : app->keywords())
      keywords.emplace_back(kw.toStdString());

    std::vector<fuzzy::WeightedField> fields;
    fields.reserve(2 + keywords.size());
    fields.push_back({name, 1.0});
    fields.push_back({desc, 0.5});
    for (const auto &kw : keywords)
      fields.push_back({kw, 0.3});

    return fuzzy::scoreWeighted(fields, query);
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
  QVariantList displayAccessory(const AppPtr &app) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const AppPtr &app) const override;
  QString sectionLabel() const override;
};
