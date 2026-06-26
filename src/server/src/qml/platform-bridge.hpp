#pragma once
#include "capabilities.hpp"
#include <QObject>
#include <QString>

/**
 * Exposes platform capabilities to QML so views can gate platform-specific
 * controls with `Platform.supports("someCapability")`.
 */
class PlatformBridge : public QObject {
  Q_OBJECT

public:
  explicit PlatformBridge(QObject *parent = nullptr) : QObject(parent) {}

  Q_INVOKABLE bool supports(const QString &capability) const {
    return platform::supports(capability.toStdString());
  }
};
