#pragma once
#include "ui/qml-engine-scope.hpp"
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
  QML_NAMED_ELEMENT(Keyboard)
  QML_SINGLETON

public:
  explicit KeyboardBridge(QObject *parent) : QObject(parent) {}
  static KeyboardBridge *create(QQmlEngine *, QJSEngine *) {
    return QmlEngineScope::global<KeyboardBridge>();
  }

private:
  Q_PROPERTY(int physicalCtrlModifier READ physicalCtrlModifier CONSTANT)

public:
  int physicalCtrlModifier() const { return static_cast<int>(KeyBindingService::PHYSICAL_CTRL); }

  Q_INVOKABLE int resolveKey(int key, int scanCode) const {
    return static_cast<int>(Keyboard::resolveKey(static_cast<Qt::Key>(key), static_cast<quint32>(scanCode)));
  }

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
    if (!shortcut.hasMods() && !shortcut.isFunctionKey()) return tr("Modifier required");
    return {};
  }
};
