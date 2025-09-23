#include "keybinding-selector.hpp"
#include <memory>

KeyBindingSelector::KeyBindingSelector() {
  std::vector<KeyBindingInfo> keybindings = {
      {"default", "Default", "Vim-style navigation (Ctrl+J/K, Ctrl+H/L)"},
      {"emacs", "Emacs", "Emacs-style navigation (Ctrl+N/P, Ctrl+B/F) and editing in search (Ctrl+A/E/K/U, Alt+B/F/Backspace/Delete)"}
  };

  std::vector<std::shared_ptr<AbstractItem>> items;
  for (const auto &keybinding : keybindings) {
    items.push_back(std::make_shared<KeyBindingSelectorItem>(keybinding));
  }

  addSection("", items);
  updateModel();
}
