#pragma once
#include <QString>

namespace Keyboard {
class Shortcut;
}

namespace shortcut_conflict {
// Validates a candidate shortcut for the owner `excludeId`, checking both in-app keybinds and global
// shortcuts (a registered global hotkey swallows the combo even for the focused app). Returns a
// user-facing error string, or empty if the shortcut is free.
QString validate(const Keyboard::Shortcut &shortcut, const QString &excludeId);
} // namespace shortcut_conflict
