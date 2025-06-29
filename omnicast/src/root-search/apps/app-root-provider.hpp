#pragma once
#include "services/app-service/app-service.hpp"
#include "app/app-database.hpp"
#include "root-item-manager.hpp"
#include <qjsonobject.h>
#include <qwidget.h>

class AppRootItem : public RootItem {
  std::shared_ptr<Application> m_app;

  double baseScoreWeight() const override;
  QString providerId() const override;
  QString typeDisplayName() const override;
  QString displayName() const override;
  ActionPanelView *actionPanel() const override;
  AccessoryList accessories() const override;
  QString uniqueId() const override;
  OmniIconUrl iconUrl() const override;
  QWidget *settingsDetail(const QJsonObject &preferences) const override;
  std::vector<QString> keywords() const override;

public:
  const Application &app() const { return *m_app.get(); }
  AppRootItem(const std::shared_ptr<Application> &app) : m_app(app) {}
};

class AppRootProvider : public RootProvider {
public:
  AppService &m_appService;

  std::vector<std::shared_ptr<RootItem>> loadItems() const override;

  QJsonObject generateDefaultPreferences() const override;
  Type type() const override;
  OmniIconUrl icon() const override;
  QString displayName() const override;
  QString uniqueId() const override;
  QWidget *settingsDetail() const override;
  void preferencesChanged(const QJsonObject &preferences) override;

public:
  AppRootProvider(AppService &appService);
};
