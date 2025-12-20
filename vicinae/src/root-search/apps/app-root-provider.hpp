#pragma once
#include "common.hpp"
#include "services/app-service/app-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include <qjsonobject.h>
#include <qwidget.h>

class AppRootItem : public RootItem {
  std::shared_ptr<AbstractApplication> m_app;

  double baseScoreWeight() const override;
  QString typeDisplayName() const override;
  QString displayName() const override;
  QString subtitle() const override;
  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override;
  AccessoryList accessories() const override;
  EntrypointId uniqueId() const override;
  ImageURL iconUrl() const override;
  QWidget *settingsDetail(const QJsonObject &preferences) const override;
  std::vector<QString> keywords() const override;
  bool isActive() const override;

public:
  const AbstractApplication &app() const { return *m_app.get(); }
  AppRootItem(const std::shared_ptr<AbstractApplication> &app) : m_app(app) {}
};

class AppRootProvider : public RootProvider {
public:
  AppService &m_appService;

  std::vector<std::shared_ptr<RootItem>> loadItems() const override;

  QJsonObject generateDefaultPreferences() const override;
  Type type() const override;
  ImageURL icon() const override;
  QString displayName() const override;
  QString uniqueId() const override;
  PreferenceList preferences() const override;
  std::optional<QJsonObject> patchPreferences(const QJsonObject &values) override;
  void preferencesChanged(const QJsonObject &preferences) override;

public:
  AppRootProvider(AppService &appService);
};
