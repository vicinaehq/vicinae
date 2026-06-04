#pragma once
#include <QObject>
#include "keyboard/keyboard.hpp"
#include "service-registry.hpp"
#include "services/global-shortcuts/global-shortcut-service.hpp"
#include "shortcut-conflict.hpp"

// Exposes global-shortcut control to QML. Registered as the "GlobalShortcuts" context property.
class GlobalShortcutBridge : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString toggleId READ toggleId CONSTANT)

public:
  using QObject::QObject;

  QString toggleId() const { return QString::fromUtf8(GlobalShortcutService::TOGGLE_ID); }

  // Suspends global binds while a recorder captures so it doesn't hijack the keystroke; rebinds on
  // release.
  Q_INVOKABLE void setCapturing(bool capturing) {
    if (auto *service = ServiceRegistry::instance()->globalShortcuts()) { service->setCapturing(capturing); }
  }

  Q_INVOKABLE QString validate(int key, int modifiers, const QString &excludeId) {
    Keyboard::Shortcut const shortcut(static_cast<Qt::Key>(key),
                                      static_cast<Qt::KeyboardModifiers>(modifiers));

    if (auto error = shortcut_conflict::validate(shortcut, excludeId); !error.isEmpty()) { return error; }

    if (auto *service = ServiceRegistry::instance()->globalShortcuts()) {
      if (auto denied = service->probeBind(shortcut)) { return *denied; }
    }

    return {};
  }
};
