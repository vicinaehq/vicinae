#pragma once
#include "fuzzy-section.hpp"
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

class BrowseAppsSection : public FuzzySection<AppPtr> {
public:
  QString sectionName() const override { return QStringLiteral("Applications ({count})"); }

protected:
  QString displayTitle(const AppPtr &app) const override;
  QString displaySubtitle(const AppPtr &app) const override;
  QString displayIconSource(const AppPtr &app) const override;
  QVariantList displayAccessories(const AppPtr &app) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const AppPtr &app) const override;
};
