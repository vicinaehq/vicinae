#pragma once
#include "fuzzy-list-model.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

using RootItemPtr = std::shared_ptr<RootItem>;

template <> struct fuzzy::FuzzySearchable<RootItemPtr> {
  static int score(const RootItemPtr &item, std::string_view query) {
    auto name = item->title().toStdString();

    std::vector<std::string> keywords;
    keywords.reserve(item->keywords().size());
    for (const auto &kw : item->keywords())
      keywords.emplace_back(kw.toStdString());

    std::vector<fuzzy::WeightedField> fields;
    fields.reserve(1 + keywords.size());
    fields.push_back({name, 1.0});
    for (const auto &kw : keywords)
      fields.push_back({kw, 0.3});

    return fuzzy::scoreWeighted(fields, query);
  }
};

class ManageFallbackModel : public FuzzyListModel<RootItemPtr> {
  Q_OBJECT

public:
  using FuzzyListModel::FuzzyListModel;

  void setFallbackItems(std::vector<RootItemPtr> fallbacks) { m_fallbacks = std::move(fallbacks); }

protected:
  void applyFilter() override;
  QString displayTitle(const RootItemPtr &item) const override;
  QString displaySubtitle(const RootItemPtr &item) const override;
  QString displayIconSource(const RootItemPtr &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const RootItemPtr &item) const override;

private:
  bool isFallbackEnabled(const RootItemPtr &item) const;
  std::vector<RootItemPtr> m_fallbacks;
};
