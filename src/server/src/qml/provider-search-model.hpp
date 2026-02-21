#pragma once
#include "command-list-model.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

class ProviderSearchModel : public CommandListModel {
  Q_OBJECT

public:
  using CommandListModel::CommandListModel;

  void setItems(std::vector<RootItemManager::ScoredItem> items);
  void setFilter(const QString &) override {}

protected:
  QString itemTitle(int section, int item) const override;
  QString itemSubtitle(int section, int item) const override;
  QString itemIconSource(int section, int item) const override;
  std::unique_ptr<ActionPanelState> createActionPanel(int section, int item) const override;

private:
  const RootItem *itemAt(int item) const;
  std::vector<RootItemManager::ScoredItem> m_items;
};
