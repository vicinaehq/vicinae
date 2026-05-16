#include <unordered_map>
#include "extend/action-model.hpp"

namespace {

const std::unordered_map<QString, Keybind> NAMED_SHORTCUT_MAP = {
    {"copy", Keybind::CopyAction},
    {"copy-deeplink", Keybind::CopyAction},
    {"copy-name", Keybind::CopyNameAction},
    {"copy-path", Keybind::CopyPathAction},
    {"save", Keybind::SaveAction},
    {"duplicate", Keybind::DuplicateAction},
    {"edit", Keybind::EditAction},
    {"move-down", Keybind::MoveDownAction},
    {"move-up", Keybind::MoveUpAction},
    {"new", Keybind::NewAction},
    {"open", Keybind::OpenAction},
    {"open-with", Keybind::OpenAction},
    {"pin", Keybind::PinAction},
    {"refresh", Keybind::RefreshAction},
    {"remove", Keybind::RemoveAction},
    {"remove-all", Keybind::DangerousRemoveAction},
};

} // namespace

Keyboard::Shortcut ActionPannelParser::parseKeyboardShortcut(const glz::generic &shortcut) {
  if (shortcut.is_string()) {
    auto str = QString::fromStdString(shortcut.get_string());

    if (auto it = NAMED_SHORTCUT_MAP.find(str); it != NAMED_SHORTCUT_MAP.end()) { return it->second; }
    return Keyboard::Shortcut::fromString(str);
  }

  if (!shortcut.is_object()) return {};
  const auto &obj = shortcut.get_object();

  auto keyIt = obj.find("key");
  if (keyIt == obj.end() || !keyIt->second.is_string()) return {};
  auto key = Keyboard::keyFromString(QString::fromStdString(keyIt->second.get_string()));
  if (!key) return {};

  Qt::KeyboardModifiers modifiers{};
  auto modsIt = obj.find("modifiers");
  if (modsIt != obj.end() && modsIt->second.is_array()) {
    for (const auto &modifierValue : modsIt->second.get_array()) {
      if (!modifierValue.is_string()) continue;
      auto modifier = Keyboard::modifierFromString(QString::fromStdString(modifierValue.get_string()));
      if (!modifier) return {};
      modifiers.setFlag(*modifier);
    }
  }

  return Keyboard::Shortcut(*key, modifiers);
}
