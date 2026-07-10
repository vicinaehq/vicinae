#pragma once
#include <QKeyEvent>
#include <string>
#include <qevent.h>

enum class KeyBindingMode { Native, Vim, Emacs };

class KeyBindingService {
public:
  // Qt swaps Control and Meta on macOS, so Qt::MetaModifier is the physical Control key there.
  // Navigation chords are physical-Control idioms on every platform; regular shortcuts keep the swap.
#ifdef Q_OS_MACOS
  static constexpr Qt::KeyboardModifier PHYSICAL_CTRL = Qt::MetaModifier;
  static constexpr bool NATIVE_NAV_CHORDS = true;
#else
  static constexpr Qt::KeyboardModifier PHYSICAL_CTRL = Qt::ControlModifier;
  static constexpr bool NATIVE_NAV_CHORDS = false;
#endif

  // "default" resolves to the platform scheme: vim-style on Linux, native keys on macOS.
  static KeyBindingMode getMode(const std::string &keybinding) {
    if (keybinding == "vim") { return KeyBindingMode::Vim; }
    if (keybinding == "emacs") { return KeyBindingMode::Emacs; }
#ifdef Q_OS_MACOS
    return KeyBindingMode::Native;
#else
    return KeyBindingMode::Vim;
#endif
  }

  static bool usesOnly(QKeyEvent *event, Qt::KeyboardModifiers required) {
    auto mods = event->modifiers();
    return (mods & required) == required &&
           (mods & ~(required | Qt::KeypadModifier | Qt::GroupSwitchModifier)) == 0;
  }

  static bool isDownKey(QKeyEvent *event, const std::string &keybinding) {
    switch (getMode(keybinding)) {
    case KeyBindingMode::Native:
      return NATIVE_NAV_CHORDS && usesOnly(event, PHYSICAL_CTRL) && event->key() == Qt::Key_N;
    case KeyBindingMode::Vim:
      return usesOnly(event, PHYSICAL_CTRL) && event->key() == Qt::Key_J;
    case KeyBindingMode::Emacs:
      return usesOnly(event, PHYSICAL_CTRL) && event->key() == Qt::Key_N;
    }
    return false;
  }

  static bool isUpKey(QKeyEvent *event, const std::string &keybinding) {
    switch (getMode(keybinding)) {
    case KeyBindingMode::Native:
      return NATIVE_NAV_CHORDS && usesOnly(event, PHYSICAL_CTRL) && event->key() == Qt::Key_P;
    case KeyBindingMode::Vim:
      return usesOnly(event, PHYSICAL_CTRL) && event->key() == Qt::Key_K;
    case KeyBindingMode::Emacs:
      return usesOnly(event, PHYSICAL_CTRL) && event->key() == Qt::Key_P;
    }
    return false;
  }

  static bool isLeftKey(QKeyEvent *event, const std::string &keybinding) {
    switch (getMode(keybinding)) {
    case KeyBindingMode::Native:
      return false;
    case KeyBindingMode::Vim:
      return usesOnly(event, PHYSICAL_CTRL) && event->key() == Qt::Key_H;
    case KeyBindingMode::Emacs:
      return usesOnly(event, PHYSICAL_CTRL | Qt::AltModifier) && event->key() == Qt::Key_B;
    }
    return false;
  }

  static bool isRightKey(QKeyEvent *event, const std::string &keybinding) {
    switch (getMode(keybinding)) {
    case KeyBindingMode::Native:
      return false;
    case KeyBindingMode::Vim:
      return usesOnly(event, PHYSICAL_CTRL) && event->key() == Qt::Key_L;
    case KeyBindingMode::Emacs:
      return usesOnly(event, PHYSICAL_CTRL | Qt::AltModifier) && event->key() == Qt::Key_F;
    }
    return false;
  }

  // Returns 0=none, 1=up, 2=down, 3=left, 4=right
  static int matchNavigation(int key, int modifiers, const std::string &keybinding) {
    QKeyEvent ev(QEvent::KeyPress, key, static_cast<Qt::KeyboardModifiers>(modifiers));
    if (isUpKey(&ev, keybinding)) return 1;
    if (isDownKey(&ev, keybinding)) return 2;
    if (isLeftKey(&ev, keybinding)) return 3;
    if (isRightKey(&ev, keybinding)) return 4;
    return 0;
  }
};
