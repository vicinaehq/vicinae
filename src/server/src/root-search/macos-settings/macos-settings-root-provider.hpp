#pragma once
#include "services/root-item-manager/root-item-manager.hpp"
#include <QCoreApplication>
#include <filesystem>

struct MacSettingsPane {
  std::filesystem::path bundlePath;
  QString displayName;
  QString bundleId;
  QString legacyBundleId;
};

class MacSettingsRootItem : public RootItem {
  Q_DECLARE_TR_FUNCTIONS(MacSettingsRootItem)

  MacSettingsPane m_pane;

  QString title() const override;
  QString typeDisplayName() const override;
  ImageURL iconUrl() const override;
  EntrypointId uniqueId() const override;
  AccessoryList accessories() const override;
  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override;
  std::vector<std::pair<QString, QString>> settingsMetadata() const override;

public:
  explicit MacSettingsRootItem(MacSettingsPane pane) : m_pane(std::move(pane)) {}
};

class MacSettingsRootProvider : public RootProvider {
public:
  QString uniqueId() const override;
  QString displayName() const override;
  ImageURL icon() const override;
  Type type() const override;
  std::vector<std::shared_ptr<RootItem>> loadItems() const override;
};
