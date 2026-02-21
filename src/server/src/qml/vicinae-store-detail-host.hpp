#pragma once
#include "bridge-view.hpp"
#include "services/extension-store/vicinae-store.hpp"
#include <QVariantList>
#include <QVariantMap>

class VicinaeStoreDetailHost : public FormViewBase {
  Q_OBJECT

signals:
  void extensionChanged();

public:
  Q_PROPERTY(QString title READ title NOTIFY extensionChanged)
  Q_PROPERTY(QString description READ description NOTIFY extensionChanged)
  Q_PROPERTY(QString iconSource READ iconSource NOTIFY extensionChanged)
  Q_PROPERTY(QString authorName READ authorName NOTIFY extensionChanged)
  Q_PROPERTY(QString authorAvatar READ authorAvatar NOTIFY extensionChanged)
  Q_PROPERTY(QString downloadCount READ downloadCount NOTIFY extensionChanged)
  Q_PROPERTY(QStringList platforms READ platforms NOTIFY extensionChanged)
  Q_PROPERTY(bool isInstalled READ isInstalled NOTIFY extensionChanged)
  Q_PROPERTY(bool hasScreenshots READ hasScreenshots NOTIFY extensionChanged)
  Q_PROPERTY(QStringList screenshots READ screenshots NOTIFY extensionChanged)
  Q_PROPERTY(QVariantList commands READ commands NOTIFY extensionChanged)
  Q_PROPERTY(QString readmeUrl READ readmeUrl NOTIFY extensionChanged)
  Q_PROPERTY(QString sourceUrl READ sourceUrl NOTIFY extensionChanged)
  Q_PROPERTY(QString lastUpdate READ lastUpdate NOTIFY extensionChanged)
  Q_PROPERTY(QVariantList contributors READ contributors NOTIFY extensionChanged)
  Q_PROPERTY(QStringList categories READ categories NOTIFY extensionChanged)
  Q_PROPERTY(QVariantMap alert READ alert NOTIFY extensionChanged)

  explicit VicinaeStoreDetailHost(const VicinaeStore::Extension &extension);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;

  QString title() const;
  QString description() const;
  QString iconSource() const;
  QString authorName() const;
  QString authorAvatar() const;
  QString downloadCount() const;
  QStringList platforms() const;
  bool isInstalled() const;
  bool hasScreenshots() const;
  QStringList screenshots() const;
  QVariantList commands() const;
  QString readmeUrl() const;
  QString sourceUrl() const;
  QString lastUpdate() const;
  QVariantList contributors() const;
  QStringList categories() const;
  QVariantMap alert() const;

  Q_INVOKABLE void openUrl(const QString &url);

private:
  QString initialNavigationTitle() const override;
  void createActions();

  VicinaeStore::Extension m_ext;
  bool m_isInstalled = false;
};
