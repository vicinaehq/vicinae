#pragma once
// We use our own shortcut stuff by design, instead of using QShortcut and the likes.
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include "keybind.hpp"

namespace Keyboard {
std::optional<QString> stringForKey(Qt::Key key);

class Shortcut {
public:
  static Shortcut osCopy() { return Shortcut(Qt::Key_C, Qt::ControlModifier); }
  static Shortcut osPaste() { return Shortcut(Qt::Key_V, Qt::ControlModifier); }
  static Shortcut enter() { return Qt::Key_Return; }
  static Shortcut submit() { return enter().shifted(); }

  static Shortcut shiftPaste() { return osPaste().shifted(); }
  static Shortcut fromString(const QString &str) { return str; }
  static Shortcut fromKeyPress(const QKeyEvent &event) { return Shortcut(&event); }

  Shortcut() : m_isValid(false) {}
  Shortcut(Qt::Key key, Qt::KeyboardModifiers mods = {}) : m_key(key), m_modifiers(mods), m_isValid(true) {}
  Shortcut(const QKeyEvent *event)
      : m_key(static_cast<Qt::Key>(event->key())), m_modifiers(event->modifiers()), m_isValid(true) {}

  /**
   * Construct shortcut from a named keybind, which are application keybinds that are configurable by the user
   */
  Shortcut(Keybind bind);
  Shortcut(const QString &str);

  Qt::Key key() const { return m_key; }
  Qt::KeyboardModifiers mods() const { return m_modifiers; }
  bool hasMods() const { return mods().toInt() > 0; }

  Shortcut shifted() { return Shortcut(m_key, m_modifiers |= Qt::ShiftModifier); }

  bool isValid() const { return m_isValid; }
  operator bool() const { return isValid(); }

  /**
   * All unique keys in the shortcut, including modifiers.
   * For instance, ctrl+shift+a returns {Qt::Key_Control, Qt::Key_Shift, Qt::Key_A}
   */
  std::vector<Qt::Key> allKeys() const;
  std::vector<Qt::Key> modKeys() const;
  std::vector<Qt::KeyboardModifier> modList() const;

  bool isValidKey() const { return stringForKey(m_key).has_value(); }
  bool isFunctionKey() const { return m_key >= Qt::Key_F1 && m_key <= Qt::Key_F12; }

  // The keyboard shortcut as a string.
  // This form is used to serialize shortcut data in config files/database.
  // It can be parsed back using Shortcut::fromString
  // e.g meta+shift+A
  QString toString() const;
  Shortcut &modded(Qt::KeyboardModifier mod);

  bool equals(const Shortcut &other, bool ignoreNumpadMod = true) const;

  bool operator==(const Shortcut &other) const;

private:
  Qt::Key m_key = Qt::Key_unknown;
  Qt::KeyboardModifiers m_modifiers;
  bool m_isValid = false;
};

}; // namespace Keyboard
