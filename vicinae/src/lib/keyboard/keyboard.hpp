#pragma once
// We use our own shortcut stuff by design, instead of using QShortcut and the likes.
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <unordered_map>
#include "keybind.hpp"

// clang-format off
static const std::unordered_map<QString, Qt::Key> keyMap = {
	{"a", Qt::Key_A},
	{"b", Qt::Key_B},
	{"c", Qt::Key_C},
	{"d", Qt::Key_D},
	{"e", Qt::Key_E},
	{"f", Qt::Key_F},
	{"g", Qt::Key_G},
	{"h", Qt::Key_H},
	{"i", Qt::Key_I},
	{"j", Qt::Key_J},
	{"k", Qt::Key_K},
	{"l", Qt::Key_L},
	{"m", Qt::Key_M},
	{"n", Qt::Key_N},
	{"o", Qt::Key_O},
	{"p", Qt::Key_P},
	{"q", Qt::Key_Q},
	{"r", Qt::Key_R},
	{"s", Qt::Key_S},
	{"t", Qt::Key_T},
	{"u", Qt::Key_U},
	{"v", Qt::Key_V},
	{"w", Qt::Key_W},
	{"x", Qt::Key_X},
	{"y", Qt::Key_Y},
	{"z", Qt::Key_Z},

	{"0", Qt::Key_0},
	{"1", Qt::Key_1},
	{"2", Qt::Key_2},
	{"3", Qt::Key_3},
	{"4", Qt::Key_4},
	{"5", Qt::Key_5},
	{"6", Qt::Key_6},
	{"7", Qt::Key_7},
	{"8", Qt::Key_8},
	{"9", Qt::Key_9},

	{".", Qt::Key_Period},
	{",", Qt::Key_Comma},
	{";", Qt::Key_Semicolon},
	{"=", Qt::Key_Equal},
	{"+", Qt::Key_Plus},
	{"-", Qt::Key_Minus},
	{"[", Qt::Key_BracketLeft},
	{"]", Qt::Key_BracketRight},
	{"{", Qt::Key_BraceLeft},
	{"}", Qt::Key_BraceRight},
	{"(", Qt::Key_ParenLeft},
	{")", Qt::Key_ParenRight},
	{"/", Qt::Key_Slash},
	{"\\", Qt::Key_Backslash},
	{"'", Qt::Key_Apostrophe},
	{"`", Qt::Key_QuoteLeft},
	{"^", Qt::Key_AsciiCircum},
	{"@", Qt::Key_At},
	{"$", Qt::Key_Dollar},

	{"return", Qt::Key_Return},
	{"delete", Qt::Key_Delete},
	{"deleteForward", Qt::Key_Backspace},
	{"tab", Qt::Key_Tab},
	{"arrowUp", Qt::Key_Up},
	{"arrowDown", Qt::Key_Down},
	{"arrowLeft", Qt::Key_Left},
	{"arrowRight", Qt::Key_Right},
	{"pageUp", Qt::Key_PageUp},
	{"pageDown", Qt::Key_PageDown},
	{"home", Qt::Key_Home},
	{"end", Qt::Key_End},
	{"space", Qt::Key_Space},
	{"escape", Qt::Key_Escape},
	{"enter", Qt::Key_Enter},
	{"backspace", Qt::Key_Backspace},

	{"f1", Qt::Key_F1},
	{"f2", Qt::Key_F2},
	{"f3", Qt::Key_F3},
	{"f4", Qt::Key_F4},
	{"f5", Qt::Key_F5},
	{"f6", Qt::Key_F6},
	{"f7", Qt::Key_F7},
	{"f8", Qt::Key_F8},
	{"f9", Qt::Key_F9},
	{"f10", Qt::Key_F10},
	{"f11", Qt::Key_F11},
	{"f12", Qt::Key_F12},
};

static const std::unordered_map<Qt::Key, QString> keyMapReverse{
	{Qt::Key_A, "a"},
	{Qt::Key_B, "b"},
	{Qt::Key_C, "c"},
	{Qt::Key_D, "d"},
	{Qt::Key_E, "e"},
	{Qt::Key_F, "f"},
	{Qt::Key_G, "g"},
	{Qt::Key_H, "h"},
	{Qt::Key_I, "i"},
	{Qt::Key_J, "j"},
	{Qt::Key_K, "k"},
	{Qt::Key_L, "l"},
	{Qt::Key_M, "m"},
	{Qt::Key_N, "n"},
	{Qt::Key_O, "o"},
	{Qt::Key_P, "p"},
	{Qt::Key_Q, "q"},
	{Qt::Key_R, "r"},
	{Qt::Key_S, "s"},
	{Qt::Key_T, "t"},
	{Qt::Key_U, "u"},
	{Qt::Key_V, "v"},
	{Qt::Key_W, "w"},
	{Qt::Key_X, "x"},
	{Qt::Key_Y, "y"},
	{Qt::Key_Z, "z"},
	{Qt::Key_0, "0"},
	{Qt::Key_1, "1"},
	{Qt::Key_2, "2"},
	{Qt::Key_3, "3"},
	{Qt::Key_4, "4"},
	{Qt::Key_5, "5"},
	{Qt::Key_6, "6"},
	{Qt::Key_7, "7"},
	{Qt::Key_8, "8"},
	{Qt::Key_9, "9"},
	{Qt::Key_Period, "."},
	{Qt::Key_Comma, ","},
	{Qt::Key_Semicolon, ";"},
	{Qt::Key_Equal, "="},
	{Qt::Key_Plus, "+"},
	{Qt::Key_Minus, "-"},
	{Qt::Key_BracketLeft, "["},
	{Qt::Key_BracketRight, "]"},
	{Qt::Key_BraceLeft, "{"},
	{Qt::Key_BraceRight, "}"},
	{Qt::Key_ParenLeft, "("},
	{Qt::Key_ParenRight, ")"},
	{Qt::Key_Slash, "/"},
	{Qt::Key_Backslash, "\\"},
	{Qt::Key_Apostrophe, "'"},
	{Qt::Key_QuoteLeft, "`"},
	{Qt::Key_AsciiCircum, "^"},
	{Qt::Key_At, "@"},
	{Qt::Key_Dollar, "$"},
	{Qt::Key_Return, "return"},
	{Qt::Key_Delete, "delete"},
	{Qt::Key_Backspace, "deleteForward"},
	{Qt::Key_Tab, "tab"},
	{Qt::Key_Up, "arrowUp"},
	{Qt::Key_Down, "arrowDown"},
	{Qt::Key_Left, "arrowLeft"},
	{Qt::Key_Right, "arrowRight"},
	{Qt::Key_PageUp, "pageUp"},
	{Qt::Key_PageDown, "pageDown"},
	{Qt::Key_Home, "home"},
	{Qt::Key_End, "end"},
	{Qt::Key_Space, "space"},
	{Qt::Key_Escape, "escape"},
	{Qt::Key_Enter, "enter"},
	{Qt::Key_Backspace, "backspace"},

	{Qt::Key_F1, "F1"},
	{Qt::Key_F2, "F2"},
	{Qt::Key_F3, "F3"},
	{Qt::Key_F4, "F4"},
	{Qt::Key_F5, "F5"},
	{Qt::Key_F6, "F6"},
	{Qt::Key_F7, "F7"},
	{Qt::Key_F8, "F8"},
	{Qt::Key_F9, "F9"},
	{Qt::Key_F10, "F10"},
	{Qt::Key_F11, "F11"},
	{Qt::Key_F12, "F12"},
};


static const std::unordered_map<QString, Qt::KeyboardModifier> modifierMap = {
	{"cmd", Qt::MetaModifier},
	{"command", Qt::MetaModifier},
	{"super", Qt::MetaModifier},
	{"meta", Qt::MetaModifier},
	{"ctrl", Qt::ControlModifier},
	{"control", Qt::ControlModifier},
	{"option", Qt::AltModifier},
	{"opt", Qt::AltModifier},
	{"alt", Qt::AltModifier},
	{"shift", Qt::ShiftModifier},
};

// clang-format on

namespace Keyboard {

inline std::optional<QString> stringForKey(Qt::Key key) {
  if (auto it = keyMapReverse.find(key); it != keyMapReverse.end()) return it->second;
  return {};
}

class Shortcut {
public:
  static Shortcut osCopy() { return Shortcut(Qt::Key_C, Qt::ControlModifier); }
  static Shortcut osPaste() { return Shortcut(Qt::Key_P, Qt::ControlModifier); }
  static Shortcut enter() { return Qt::Key_Return; }
  static Shortcut submit() { return enter().shifted(); }

  static Shortcut shiftPaste() { return osPaste().shifted(); }
  static Shortcut fromString(const QString &str) { return str; }
  static Shortcut fromKeyPress(const QKeyEvent &event) { return Shortcut(&event); }

  Shortcut() { m_isValid = false; }
  Shortcut(Qt::Key key, Qt::KeyboardModifiers mods = {}) : m_key(key), m_modifiers(mods) {}
  Shortcut(const QKeyEvent *event)
      : m_key(static_cast<Qt::Key>(event->key())), m_modifiers(event->modifiers()) {}

  /**
   * Construct shortcut from a named keybind, which are application keybinds that are configurable by the user
   */
  Shortcut(Keybind bind);

  Shortcut(const QString &str) {
    auto sequence = QKeySequence::fromString(str);

    auto strs = str.split('+', Qt::SkipEmptyParts);
    bool gotKey = false;

    for (const auto &str : strs) {
      if (auto it = modifierMap.find(str); it != modifierMap.end()) {
        m_modifiers.setFlag(it->second);
      } else if (auto it = keyMap.find(str.toLower()); it != keyMap.end()) {
        gotKey = true;
        m_key = it->second;
      } else {
        m_isValid = false;
        return;
      }
    }

    m_isValid = gotKey;
  }

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
  std::vector<Qt::Key> allKeys() const {
    auto mods = modKeys();

    if (std::ranges::find(mods, m_key) == mods.end()) { mods.emplace_back(m_key); }

    return mods;
  }

  std::vector<Qt::Key> modKeys() const {
    std::vector<Qt::Key> keys;
    keys.reserve(4);

    if (m_modifiers.testFlag(Qt::MetaModifier)) { keys.emplace_back(Qt::Key_Meta); }
    if (m_modifiers.testFlag(Qt::ControlModifier)) { keys.emplace_back(Qt::Key_Control); }
    if (m_modifiers.testFlag(Qt::AltModifier)) { keys.emplace_back(Qt::Key_Alt); }
    if (m_modifiers.testFlag(Qt::ShiftModifier)) { keys.emplace_back(Qt::Key_Shift); }

    return keys;
  }

  std::vector<Qt::KeyboardModifier> modList() const {
    std::vector<Qt::KeyboardModifier> modifiers;

    modifiers.reserve(4);

    if (m_modifiers.testFlag(Qt::MetaModifier)) { modifiers.emplace_back(Qt::MetaModifier); }
    if (m_modifiers.testFlag(Qt::ControlModifier)) { modifiers.emplace_back(Qt::ControlModifier); }
    if (m_modifiers.testFlag(Qt::AltModifier)) { modifiers.emplace_back(Qt::AltModifier); }
    if (m_modifiers.testFlag(Qt::ShiftModifier)) { modifiers.emplace_back(Qt::ShiftModifier); }

    return modifiers;
  }

  bool isValidKey() const { return stringForKey(m_key).has_value(); }
  bool isFunctionKey() const { return m_key >= Qt::Key_F1 && m_key <= Qt::Key_F12; }

  // The keyboard shortcut as a string.
  // This form is used to serialize shortcut data in config files/database.
  // It can be parsed back using Shortcut::fromString
  // e.g meta+shift+A
  QString toString() const {
    QStringList strs;

    if (m_modifiers.testFlag(Qt::MetaModifier)) { strs << "super"; }
    if (m_modifiers.testFlag(Qt::ControlModifier)) { strs << "control"; }
    if (m_modifiers.testFlag(Qt::AltModifier)) { strs << "alt"; }
    if (m_modifiers.testFlag(Qt::ShiftModifier)) { strs << "shift"; }

    strs << stringForKey(m_key).value_or("?").toUpper();

    return strs.join('+');
  }

  Shortcut &withModifier(Qt::KeyboardModifier mod) {
    m_modifiers.setFlag(mod);
    return *this;
  }

  bool equals(const Shortcut &other, bool ignoreNumpadMod = true) const {
    if (ignoreNumpadMod) {
      return m_key == other.m_key &&
             (m_modifiers.toInt() & ~Qt::KeypadModifier) == (other.m_modifiers.toInt() & ~Qt::KeypadModifier);
    }

    return m_key == other.m_key && m_modifiers.toInt() == other.m_modifiers.toInt();
  }

  bool operator==(const Shortcut &other) const {
    return m_key == other.m_key && m_modifiers.toInt() == other.m_modifiers.toInt();
  }

private:
  Qt::Key m_key;
  Qt::KeyboardModifiers m_modifiers;
  bool m_isValid;
};

}; // namespace Keyboard
