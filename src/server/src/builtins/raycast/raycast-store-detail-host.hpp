#pragma once
#include "ui/views/store-detail-host.hpp"
#include "services/raycast/raycast-store.hpp"
#include <QVariantList>
#include <QVariantMap>

class RaycastStoreDetailHost : public StoreDetailHost {
  Q_OBJECT

public:
  explicit RaycastStoreDetailHost(const Raycast::Extension &extension);
  RaycastStoreDetailHost(const QString &authorHandle, const QString &extensionName);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;

  QString title() const override;
  QString description() const override;
  QString iconSource() const override;
  QString authorName() const override;
  QString authorAvatar() const override;
  QString downloadCount() const override;
  QStringList platforms() const override;
  bool isReady() const override;
  bool isInstalled() const override;
  bool hasScreenshots() const override;
  QStringList screenshots() const override;
  QVariantList commands() const override;
  QString readmeUrl() const override;
  QString sourceUrl() const override;
  QString lastUpdate() const override;
  QVariantList contributors() const override;
  QStringList categories() const override;
  QVariantMap alert() const override;

  void openUrl(const QString &url) override;

private:
  QString initialNavigationTitle() const override;
  void createActions();
  void hydrate(const Raycast::Extension &extension);
  void buildAlert();

  Raycast::Extension m_ext;
  QString m_authorHandle;
  QString m_extensionName;
  bool m_isReady = false;
  bool m_isInstalled = false;
  QVariantMap m_alert;
};
