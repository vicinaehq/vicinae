#pragma once
#include "ui/views/store-detail-host.hpp"
#include "services/extension-store/vicinae-store.hpp"
#include <QVariantList>
#include <QVariantMap>

class VicinaeStoreDetailHost : public StoreDetailHost {
  Q_OBJECT

public:
  VicinaeStoreDetailHost(const QString &authorHandle, const QString &extensionName);

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

  void openUrl(const QString &url) override;

private:
  QString initialNavigationTitle() const override;
  void createActions();

  void hydrate(const VicinaeStore::Extension &extension);

  VicinaeStore::Extension m_ext;
  QString m_authorHandle;
  QString m_extensionName;
  bool m_isReady = false;
  bool m_isInstalled = false;
};
