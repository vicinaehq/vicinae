#pragma once

#include "extend/action-model.hpp"
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>

// clang-format off
static QHash<QString, Qt::Key> keyMap = {
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
	{"backspace", Qt::Key_Backspace}
};

static QHash<QString, Qt::KeyboardModifier> modifierMap = {
	{"cmd", Qt::MetaModifier},
	{"ctrl", Qt::ControlModifier},
	{"opt", Qt::AltModifier},
	{"shift", Qt::ShiftModifier},
};

// clang-format on

struct KeyboardShortcut {
  Qt::Key key;
  Qt::KeyboardModifiers modifiers;

public:
  static KeyboardShortcut copy() {
    return KeyboardShortcut(Qt::Key_C).withModifier(Qt::KeyboardModifier::ControlModifier);
  }

  static KeyboardShortcut paste() {
    return KeyboardShortcut(Qt::Key_V).withModifier(Qt::KeyboardModifier::ControlModifier);
  }

  static KeyboardShortcut actionPanel() {
    return KeyboardShortcut(Qt::Key_B).withModifier(Qt::KeyboardModifier::ControlModifier);
  }

  static KeyboardShortcut shiftPaste() {
    return KeyboardShortcut(Qt::Key_V)
        .withModifier(Qt::KeyboardModifier::ControlModifier)
        .withModifier(Qt::KeyboardModifier::ShiftModifier);
  }

  KeyboardShortcut(const KeyboardShortcutModel &model) : key(keyMap.value(model.key.toLower())) {
    for (const auto &mod : model.modifiers) {
      modifiers.setFlag(modifierMap.value(mod));
    }
  }

  KeyboardShortcut(Qt::Key key) : key(key) {}

  KeyboardShortcut &withModifier(Qt::KeyboardModifier mod) {
    modifiers.setFlag(mod);
    return *this;
  }

  KeyboardShortcut() {}

  bool matchesKeyEvent(const QKeyEvent *event) const {
    return event->key() == key && modifiers.toInt() == event->modifiers().toInt();
  }
};
