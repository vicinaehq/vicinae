#pragma once
#include "common.hpp"
#include "services/shortcut/shortcut-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

class RootShortcutItem : public RootItem {
  std::shared_ptr<Shortcut> m_link;

  QString displayName() const override;
  double baseScoreWeight() const override;
  AccessoryList accessories() const override;
  bool isSuitableForFallback() const override;
  ArgumentList arguments() const override;
  ImageURL iconUrl() const override;
  EntrypointId uniqueId() const override;
  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override;
  std::unique_ptr<ActionPanelState> fallbackActionPanel(ApplicationContext *ctx,
                                                        const RootItemMetadata &metadata) const override;
  QString typeDisplayName() const override;

public:
  const Shortcut &shortcut() const { return *m_link.get(); }

  RootShortcutItem(const std::shared_ptr<Shortcut> &link);
};

class ShortcutRootProvider : public RootProvider {
  ShortcutService &m_db;

public:
  QString displayName() const override;
  QString uniqueId() const override;
  ImageURL icon() const override;
  Type type() const override;
  std::vector<std::shared_ptr<RootItem>> loadItems() const override;

  ShortcutRootProvider(ShortcutService &db);
};
