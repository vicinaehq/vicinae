#pragma once
#include <QKeyEvent>
#include <QString>

enum class KeyBindingMode {
  Default,
  Emacs
};

class KeyBindingService {
public:
  static KeyBindingMode getMode(const QString &keybinding) {
    if (keybinding == "emacs") {
      return KeyBindingMode::Emacs;
    }
    return KeyBindingMode::Default;
  }

  static bool isDownKey(QKeyEvent *event, const QString &keybinding) {
    KeyBindingMode mode = getMode(keybinding);
    
    if (event->modifiers() != Qt::ControlModifier) {
      return false;
    }

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
    
    if (event->modifiers() != Qt::ControlModifier) {
      return false;
    }

    switch (mode) {
    case KeyBindingMode::Default:
      return event->key() == Qt::Key_K;
    case KeyBindingMode::Emacs:
      return event->key() == Qt::Key_P;
    }
    return false;
  }

  static bool isLeftKey(QKeyEvent *event, const QString &keybinding) {
    if (event->modifiers() != Qt::ControlModifier) {
      return false;
    }
    KeyBindingMode mode = getMode(keybinding);
    switch (mode) {
    case KeyBindingMode::Default:
      return event->key() == Qt::Key_H;
    case KeyBindingMode::Emacs:
      return event->key() == Qt::Key_B;
    }
    return false;
  }

  static bool isRightKey(QKeyEvent *event, const QString &keybinding) {
    if (event->modifiers() != Qt::ControlModifier) {
      return false;
    }
    KeyBindingMode mode = getMode(keybinding);
    switch (mode) {
    case KeyBindingMode::Default:
      return event->key() == Qt::Key_L;
    case KeyBindingMode::Emacs:
      return event->key() == Qt::Key_F;
    }
    return false;
  }
};
