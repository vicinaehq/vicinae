#pragma once
#include <QObject>
#include <QVariantList>
#include "keyboard/keyboard.hpp"
#include "config/config.hpp"
#include "service-registry.hpp"
#include "services/keybinding/keybinding-service.hpp"

/**
 * Exposes the generic Keyboard::Shortcut conversions to QML so shortcut widgets don't have to
 * reimplement them per model. Registered as the "Keyboard" context property.
 */
class KeyboardBridge : public QObject {
  Q_OBJECT

public:
  using QObject::QObject;

  Q_INVOKABLE QString serialize(int key, int modifiers) const {
    Keyboard::Shortcut const shortcut(static_cast<Qt::Key>(key),
                                      static_cast<Qt::KeyboardModifiers>(modifiers));
    return shortcut.isValid() ? shortcut.toString() : QString();
  }

  Q_INVOKABLE QVariantList tokens(int key, int modifiers) const {
    return Keyboard::Shortcut(static_cast<Qt::Key>(key), static_cast<Qt::KeyboardModifiers>(modifiers))
        .toDisplayTokens();
  }

  Q_INVOKABLE QVariantList tokensForString(const QString &shortcut) const {
    if (shortcut.isEmpty()) return {};
    Keyboard::Shortcut const parsed = Keyboard::Shortcut::fromString(shortcut);
    return parsed.isValid() ? parsed.toDisplayTokens() : QVariantList{};
  }

  Q_INVOKABLE int matchNavigation(int key, int modifiers) const {
    return KeyBindingService::matchNavigation(key, modifiers,
                                              ServiceRegistry::instance()->config()->value().keybinding);
  }

  Q_INVOKABLE QString validate(int key, int modifiers) const {
    Keyboard::Shortcut const shortcut(static_cast<Qt::Key>(key),
                                      static_cast<Qt::KeyboardModifiers>(modifiers));
    if (!shortcut.hasMods() && !shortcut.isFunctionKey()) return QStringLiteral("Modifier required");
    return {};
  }
};
