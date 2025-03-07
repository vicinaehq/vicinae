#pragma once

#include "extend/action-model.hpp"
#include <qevent.h>
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
	{"cmd", Qt::KeyboardModifier::MetaModifier},
	{"ctrl", Qt::KeyboardModifier::ControlModifier},
	{"opt", Qt::KeyboardModifier::AltModifier},
	{"shift", Qt::KeyboardModifier::ShiftModifier},
};

// clang-format on

struct KeyboardShortcut {
  Qt::Key key;
  Qt::KeyboardModifiers modifiers;

public:
  KeyboardShortcut(const KeyboardShortcutModel &model) : key(keyMap.value(model.key)) {
    for (const auto &mod : model.modifiers) {
      modifiers.setFlag(modifierMap.value(mod));
    }
  }

  KeyboardShortcut() {}

  bool operator==(QKeyEvent *event) { return event->key() == key && event->modifiers() == modifiers; }
};
