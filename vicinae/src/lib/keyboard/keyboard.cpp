#include "keybind-manager.hpp"
#include "keyboard.hpp"

namespace Keyboard {
Shortcut Shortcut::copy() { return *KeybindManager::instance()->resolve(Keybind::CopyAction); }
Shortcut Shortcut::duplicate() { return *KeybindManager::instance()->resolve(Keybind::NewAction); }
Shortcut Shortcut::pin() { return *KeybindManager::instance()->resolve(Keybind::PinAction); }
Shortcut Shortcut::paste() { return Shortcut(Qt::Key_V).withModifier(Qt::KeyboardModifier::ControlModifier); }
Shortcut Shortcut::enter() { return Shortcut(Qt::Key_Return); }
Shortcut Shortcut::open() { return *KeybindManager::instance()->resolve(Keybind::OpenAction); }
Shortcut Shortcut::edit() { return *KeybindManager::instance()->resolve(Keybind::EditAction); }
Shortcut Shortcut::submit() { return Shortcut(Qt::Key_Return, Qt::ShiftModifier); }
Shortcut Shortcut::remove() { return *KeybindManager::instance()->resolve(Keybind::RemoveAction); }
Shortcut Shortcut::dangerousRemove() {
  return *KeybindManager::instance()->resolve(Keybind::DangerousRemoveAction);
}
static Shortcut actionPanel() { return *KeybindManager::instance()->resolve(Keybind::ToggleActionPanel); }
}; // namespace Keyboard
