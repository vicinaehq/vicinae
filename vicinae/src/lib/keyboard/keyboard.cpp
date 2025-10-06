#include "keybind-manager.hpp"
#include "keyboard.hpp"

namespace Keyboard {
Shortcut::Shortcut(Keybind bind) { *this = KeybindManager::instance()->resolve(bind); }
}; // namespace Keyboard
