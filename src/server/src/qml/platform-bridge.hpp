#pragma once
#include "capabilities.hpp"
#include <QGuiApplication>
#include <QPalette>
#include <QColor>
#include <QObject>
#include <QString>
#ifdef Q_OS_MACOS
#include "macos-chrome-attached.hpp"
#endif

/**
 * Exposes platform capabilities to QML so views can gate platform-specific
 * controls with `Platform.supports("someCapability")`.
 */
class PlatformBridge : public QObject {
  Q_OBJECT
  // The OS accent color (user-selectable on macOS), for controls that render
  // native chrome rather than theme tokens.
  Q_PROPERTY(QColor accentColor READ accentColor CONSTANT)

public:
  explicit PlatformBridge(QObject *parent = nullptr) : QObject(parent) {}

  QColor accentColor() const {
#ifdef Q_OS_MACOS
    return macosAccentColor();
#else
    return QGuiApplication::palette().accent().color();
#endif
  }

  Q_INVOKABLE bool supports(const QString &capability) const {
    return platform::supports(capability.toStdString());
  }

  Q_INVOKABLE bool preferItemPopup(const QString &surface) const {
    return platform::preferItemPopup(surface.toStdString());
  }
};
