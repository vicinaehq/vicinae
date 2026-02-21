#pragma once
#include "fuzzy-list-model.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

using RootItemPtr = std::shared_ptr<RootItem>;

template <> struct fuzzy::FuzzySearchable<RootItemPtr> {
  static int score(const RootItemPtr &item, std::string_view query) {
    auto name = item->displayName().toStdString();
    int best = fuzzy::scoreWeighted({{name, 1.0}}, query);
    for (const auto &kw : item->keywords()) {
      auto s = kw.toStdString();
      int kwScore = fuzzy::scoreWeighted({{s, 0.7}}, query);
      if (kwScore > best) best = kwScore;
    }
    return best;
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
