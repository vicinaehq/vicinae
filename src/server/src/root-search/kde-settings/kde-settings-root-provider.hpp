#pragma once
#include "services/app-service/app-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include <QCoreApplication>

class KdeSettingsRootItem : public RootItem {
  Q_DECLARE_TR_FUNCTIONS(KdeSettingsRootItem)

  std::shared_ptr<AbstractApplication> m_app;

  QString title() const override;
  QString typeDisplayName() const override;
  ImageURL iconUrl() const override;
  EntrypointId uniqueId() const override;
  AccessoryList accessories() const override;
  std::vector<QString> keywords() const override;
  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override;
  std::vector<std::pair<QString, QString>> settingsMetadata() const override;

public:
  explicit KdeSettingsRootItem(std::shared_ptr<AbstractApplication> app) : m_app(std::move(app)) {}
};

class KdeSettingsRootProvider : public RootProvider {
  Q_DECLARE_TR_FUNCTIONS(KdeSettingsRootProvider)

  AppService &m_appService;

public:
  QString uniqueId() const override;
  QString displayName() const override;
  QString description() const override;
  ImageURL icon() const override;
  Type type() const override;
  std::vector<std::shared_ptr<RootItem>> loadItems() const override;

  explicit KdeSettingsRootProvider(AppService &appService);
};
