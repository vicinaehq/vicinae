#pragma once
#include "ui/qml-engine-scope.hpp"
#include "capabilities.hpp"
#include <QObject>
#include <QString>

/**
 * Exposes platform capabilities to QML so views can gate platform-specific
 * controls with `Platform.supports("someCapability")`.
 */
class PlatformBridge : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(Platform)
  QML_SINGLETON

public:
  static PlatformBridge *create(QQmlEngine *, QJSEngine *) {
    return QmlEngineScope::global<PlatformBridge>();
  }

private:
public:
  explicit PlatformBridge(QObject *parent = nullptr) : QObject(parent) {}

  Q_INVOKABLE bool supports(const QString &capability) const {
    return platform::supports(capability.toStdString());
  }

  Q_INVOKABLE bool preferItemPopup(const QString &surface) const {
    return platform::preferItemPopup(surface.toStdString());
  }
};
