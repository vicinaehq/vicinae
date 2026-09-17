#pragma once
#include "ui/views/bridge-view.hpp"
#include <QVariantList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

class StoreDetailHost : public FormViewBase {
  Q_OBJECT
  QML_NAMED_ELEMENT(StoreDetailHost)
  QML_UNCREATABLE("")
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
  Q_PROPERTY(bool isReady READ isReady NOTIFY extensionChanged)

signals:
  void extensionChanged();

public:
  virtual QString title() const = 0;
  virtual QString description() const = 0;
  virtual QString iconSource() const = 0;
  virtual QString authorName() const = 0;
  virtual QString authorAvatar() const = 0;
  virtual QString downloadCount() const = 0;
  virtual QStringList platforms() const = 0;
  virtual bool isReady() const = 0;
  virtual bool isInstalled() const = 0;
  virtual bool hasScreenshots() const = 0;
  virtual QStringList screenshots() const = 0;
  virtual QVariantList commands() const = 0;
  virtual QString readmeUrl() const = 0;
  virtual QString sourceUrl() const = 0;
  virtual QString lastUpdate() const = 0;
  virtual QVariantList contributors() const = 0;
  virtual QStringList categories() const = 0;
  virtual QVariantMap alert() const { return {}; }

  Q_INVOKABLE virtual void openUrl(const QString &url) = 0;
};
