#pragma once
#include <QKeyEvent>
#include <QString>
#include <qevent.h>

enum class KeyBindingMode { Default, Emacs };

class KeyBindingService {
public:
  static KeyBindingMode getMode(const QString &keybinding) {
    if (keybinding == "emacs") { return KeyBindingMode::Emacs; }
    return KeyBindingMode::Default;
  }

  static bool usesOnly(QKeyEvent *event, Qt::KeyboardModifiers required) {
    auto mods = event->modifiers();
    return (mods & required) == required &&
           (mods & ~(required | Qt::KeypadModifier | Qt::GroupSwitchModifier)) == 0;
  }

  static bool isDownKey(QKeyEvent *event, const QString &keybinding) {
    KeyBindingMode mode = getMode(keybinding);

    if (!usesOnly(event, Qt::ControlModifier)) { return false; }

    switch (mode) {
    case KeyBindingMode::Default:
      return event->key() == Qt::Key_J;
    case KeyBindingMode::Emacs:
      return event->key() == Qt::Key_N;
    }
    return false;
  }

  static bool isUpKey(QKeyEvent *event, const QString &keybinding) {
    KeyBindingMode mode = getMode(keybinding);

    if (!usesOnly(event, Qt::ControlModifier)) { return false; }

    switch (mode) {
    case KeyBindingMode::Default:
      return event->key() == Qt::Key_K;
    case KeyBindingMode::Emacs:
      return event->key() == Qt::Key_P;
    }
    return false;
  }

  static bool isLeftKey(QKeyEvent *event, const QString &keybinding) {
    KeyBindingMode mode = getMode(keybinding);
    switch (mode) {
    case KeyBindingMode::Default:
      return usesOnly(event, Qt::ControlModifier) && event->key() == Qt::Key_H;
    case KeyBindingMode::Emacs:
      return usesOnly(event, Qt::ControlModifier | Qt::AltModifier) && event->key() == Qt::Key_B;
    }
    return false;
  }

  static bool isRightKey(QKeyEvent *event, const QString &keybinding) {
    KeyBindingMode mode = getMode(keybinding);
    switch (mode) {
    case KeyBindingMode::Default:
      return usesOnly(event, Qt::ControlModifier) && event->key() == Qt::Key_L;
    case KeyBindingMode::Emacs:
      return usesOnly(event, Qt::ControlModifier | Qt::AltModifier) && event->key() == Qt::Key_F;
    }
    return false;
  }

  static bool isSearchAccessoryKey(QKeyEvent *event, const QString &keybinding) {
    switch (getMode(keybinding)) {
    case KeyBindingMode::Emacs:
      return event->keyCombination() == QKeyCombination(Qt::AltModifier, Qt::Key_P);
    default:
      return event->keyCombination() == QKeyCombination(Qt::ControlModifier, Qt::Key_P);
    }
  }
};
