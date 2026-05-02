#pragma once
#include "fuzzy-section.hpp"
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

class EnabledFallbackSection : public FuzzySection<RootItemPtr> {
public:
  QString sectionName() const override { return QStringLiteral("Enabled"); }

  void setFallbackOrder(std::vector<RootItemPtr> fallbacks) { m_fallbacks = std::move(fallbacks); }

  void setFilter(std::string_view query) override;

protected:
  QString displayTitle(const RootItemPtr &item) const override;
  QString displaySubtitle(const RootItemPtr &item) const override;
  QString displayIconSource(const RootItemPtr &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const RootItemPtr &item) const override;

private:
  std::vector<RootItemPtr> m_fallbacks;
};

class AvailableFallbackSection : public FuzzySection<RootItemPtr> {
public:
  QString sectionName() const override { return QStringLiteral("Available"); }

protected:
  QString displayTitle(const RootItemPtr &item) const override;
  QString displaySubtitle(const RootItemPtr &item) const override;
  QString displayIconSource(const RootItemPtr &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const RootItemPtr &item) const override;
};
