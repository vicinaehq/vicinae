#include <QKeySequence>
#include <QVariantMap>
#include "keybind-manager.hpp"
#include "keyboard.hpp"

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
	{"arrowup", Qt::Key_Up},
	{"arrowdown", Qt::Key_Down},
	{"arrowleft", Qt::Key_Left},
	{"arrowright", Qt::Key_Right},
	{"pageup", Qt::Key_PageUp},
	{"pagedown", Qt::Key_PageDown},
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
	{Qt::Key_Backspace, "deleteforward"},
	{Qt::Key_Tab, "tab"},
	{Qt::Key_Up, "arrowup"},
	{Qt::Key_Down, "arrowdown"},
	{Qt::Key_Left, "arrowleft"},
	{Qt::Key_Right, "arrowright"},
	{Qt::Key_PageUp, "pageup"},
	{Qt::Key_PageDown, "pagedown"},
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

std::optional<QString> stringForKey(Qt::Key key) {
  if (auto it = keyMapReverse.find(key); it != keyMapReverse.end()) return it->second;
  return {};
}

std::optional<Qt::Key> keyFromString(QStringView key) {
  auto keyString = key.toString().toLower();
  if (auto it = keyMap.find(keyString); it != keyMap.end()) return it->second;
  return {};
}

std::optional<Qt::KeyboardModifier> modifierFromString(QStringView modifier) {
  auto modifierString = modifier.toString().toLower();
  if (auto it = modifierMap.find(modifierString); it != modifierMap.end()) return it->second;
  return {};
}

namespace {

struct DisplayTokenSpec {
  QString text;
  QString icon;
  QString label;
};

std::optional<Qt::KeyboardModifier> modifierForKey(Qt::Key key) {
  switch (key) {
  case Qt::Key_Meta:
    return Qt::MetaModifier;
  case Qt::Key_Control:
    return Qt::ControlModifier;
  case Qt::Key_Alt:
    return Qt::AltModifier;
  case Qt::Key_Shift:
    return Qt::ShiftModifier;
  default:
    return std::nullopt;
  }
}

DisplayTokenSpec modifierToken(Qt::KeyboardModifier modifier) {
  switch (modifier) {
  case Qt::MetaModifier:
    return {.text = QStringLiteral("Super"), .label = QStringLiteral("Super")};
  case Qt::ControlModifier:
    return {.text = QStringLiteral("Ctrl"), .label = QStringLiteral("Ctrl")};
  case Qt::AltModifier:
    return {.text = QStringLiteral("Alt"), .label = QStringLiteral("Alt")};
  case Qt::ShiftModifier:
    return {.icon = QStringLiteral("keyboard-shift"), .label = QStringLiteral("Shift")};
  default:
    return {};
  }
}

std::optional<DisplayTokenSpec> keyToken(Qt::Key key) {
  switch (key) {
  case Qt::Key_Return:
  case Qt::Key_Enter:
    return DisplayTokenSpec{.icon = QStringLiteral("enter-key"), .label = QStringLiteral("Enter")};
  case Qt::Key_Tab:
    return DisplayTokenSpec{.icon = QStringLiteral("tab-key"), .label = QStringLiteral("Tab")};
  case Qt::Key_Space:
    return DisplayTokenSpec{.icon = QStringLiteral("space-key"), .label = QStringLiteral("Space")};
  case Qt::Key_Backspace:
    return DisplayTokenSpec{.text = QStringLiteral("⌫"), .label = QStringLiteral("Backspace")};
  case Qt::Key_Delete:
    return DisplayTokenSpec{.text = QStringLiteral("⌦"), .label = QStringLiteral("Delete")};
  case Qt::Key_Up:
    return DisplayTokenSpec{.text = QStringLiteral("↑"), .label = QStringLiteral("Up")};
  case Qt::Key_Down:
    return DisplayTokenSpec{.text = QStringLiteral("↓"), .label = QStringLiteral("Down")};
  case Qt::Key_Left:
    return DisplayTokenSpec{.text = QStringLiteral("←"), .label = QStringLiteral("Left")};
  case Qt::Key_Right:
    return DisplayTokenSpec{.text = QStringLiteral("→"), .label = QStringLiteral("Right")};
  case Qt::Key_PageUp:
    return DisplayTokenSpec{.text = QStringLiteral("PgUp"), .label = QStringLiteral("PageUp")};
  case Qt::Key_PageDown:
    return DisplayTokenSpec{.text = QStringLiteral("PgDn"), .label = QStringLiteral("PageDown")};
  case Qt::Key_Home:
    return DisplayTokenSpec{.text = QStringLiteral("Home"), .label = QStringLiteral("Home")};
  case Qt::Key_End:
    return DisplayTokenSpec{.text = QStringLiteral("End"), .label = QStringLiteral("End")};
  case Qt::Key_Escape:
    return DisplayTokenSpec{.text = QStringLiteral("Esc"), .label = QStringLiteral("Escape")};
  default:
    break;
  }

  if (auto keyString = stringForKey(key)) {
    auto label = keyString->toUpper();
    return DisplayTokenSpec{.text = label, .label = label};
  }

  return std::nullopt;
}

std::vector<DisplayTokenSpec> buildDisplayTokenSpecs(const Shortcut &shortcut) {
  Qt::KeyboardModifiers modifiers = shortcut.mods();
  auto modifierKey = modifierForKey(shortcut.key());
  if (modifierKey) { modifiers.setFlag(*modifierKey); }

  std::vector<DisplayTokenSpec> tokens;
  tokens.reserve(5);

  auto appendModifier = [&](Qt::KeyboardModifier modifier) {
    if (modifiers.testFlag(modifier)) { tokens.emplace_back(modifierToken(modifier)); }
  };

  appendModifier(Qt::MetaModifier);
  appendModifier(Qt::ControlModifier);
  appendModifier(Qt::AltModifier);
  appendModifier(Qt::ShiftModifier);

  if (!modifierKey) {
    if (auto token = keyToken(shortcut.key())) {
      tokens.emplace_back(std::move(*token));
    } else if (tokens.empty()) {
      tokens.emplace_back(DisplayTokenSpec{.text = QStringLiteral("?"), .label = QStringLiteral("?")});
    }
  }

  return tokens;
}

} // namespace

Shortcut::Shortcut(Keybind bind) { *this = KeybindManager::instance()->resolve(bind); }

Shortcut::Shortcut(const QString &str) {
  auto strs = str.split('+', Qt::SkipEmptyParts);
  bool gotKey = false;

  for (const auto &str : strs) {
    if (auto modifier = modifierFromString(str)) {
      m_modifiers.setFlag(*modifier);
    } else if (auto key = keyFromString(str)) {
      gotKey = true;
      m_key = *key;
    } else {
      m_isValid = false;
      return;
    }
  }

  m_isValid = gotKey;
}

QString Shortcut::toString() const {
  QStringList strs;

  if (m_modifiers.testFlag(Qt::MetaModifier)) { strs << "super"; }
  if (m_modifiers.testFlag(Qt::ControlModifier)) { strs << "control"; }
  if (m_modifiers.testFlag(Qt::AltModifier)) { strs << "alt"; }
  if (m_modifiers.testFlag(Qt::ShiftModifier)) { strs << "shift"; }

  strs << stringForKey(m_key).value_or("?").toUpper();

  return strs.join('+');
}

QString Shortcut::toBindingSequence() const {
  if (!m_isValid && !modifierForKey(m_key)) return {};

  Qt::KeyboardModifiers modifiers = m_modifiers;
  auto modifierKey = modifierForKey(m_key);
  if (modifierKey) { modifiers.setFlag(*modifierKey); }

  if (modifierKey && modifiers == Qt::KeyboardModifiers(*modifierKey)) {
    return QKeySequence(modifiers.toInt()).toString(QKeySequence::PortableText);
  }

  return QKeySequence(static_cast<int>(m_key) | modifiers.toInt()).toString(QKeySequence::PortableText);
}

QString Shortcut::toDisplayString() const {
  if (!m_isValid && !modifierForKey(m_key)) return {};

  QStringList parts;
  auto tokens = buildDisplayTokenSpecs(*this);
  parts.reserve(static_cast<qsizetype>(tokens.size()));
  for (const auto &token : tokens)
    parts << token.label;

  return parts.join('+');
}

QVariantList Shortcut::toDisplayTokens() const {
  if (!m_isValid && !modifierForKey(m_key)) return {};

  QVariantList tokens;
  for (const auto &token : buildDisplayTokenSpecs(*this)) {
    QVariantMap entry;
    entry.insert(QStringLiteral("text"), token.text);
    entry.insert(QStringLiteral("icon"), token.icon);
    tokens.append(entry);
  }

  return tokens;
}

std::vector<Qt::KeyboardModifier> Shortcut::modList() const {
  std::vector<Qt::KeyboardModifier> modifiers;

  modifiers.reserve(4);

  if (m_modifiers.testFlag(Qt::MetaModifier)) { modifiers.emplace_back(Qt::MetaModifier); }
  if (m_modifiers.testFlag(Qt::ControlModifier)) { modifiers.emplace_back(Qt::ControlModifier); }
  if (m_modifiers.testFlag(Qt::AltModifier)) { modifiers.emplace_back(Qt::AltModifier); }
  if (m_modifiers.testFlag(Qt::ShiftModifier)) { modifiers.emplace_back(Qt::ShiftModifier); }

  return modifiers;
}

Shortcut &Shortcut::modded(Qt::KeyboardModifier mod) {
  m_modifiers.setFlag(mod);
  return *this;
}

bool Shortcut::equals(const Shortcut &other, bool ignoreNumpadMod) const {
  if (ignoreNumpadMod) {
    return m_key == other.m_key &&
           (m_modifiers.toInt() & ~Qt::KeypadModifier) == (other.m_modifiers.toInt() & ~Qt::KeypadModifier);
  }

  return m_key == other.m_key && m_modifiers.toInt() == other.m_modifiers.toInt();
}

bool Shortcut::operator==(const Shortcut &other) const {
  return m_key == other.m_key && m_modifiers.toInt() == other.m_modifiers.toInt();
}

}; // namespace Keyboard
