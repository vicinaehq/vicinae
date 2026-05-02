#pragma once
#include "section-source.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

class ProviderSearchSection : public SectionSource {
public:
  void setItems(std::vector<RootItemManager::ScoredItem> items);

  QString sectionName() const override { return QStringLiteral("Results ({count})"); }
  int count() const override { return static_cast<int>(m_items.size()); }

protected:
  QString itemTitle(int i) const override;
  QString itemSubtitle(int i) const override;
  QString itemIconSource(int i) const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  const RootItem *itemAt(int i) const;
  std::vector<RootItemManager::ScoredItem> m_items;
};
