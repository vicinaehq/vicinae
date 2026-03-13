#include <unordered_map>
#include "extend/action-model.hpp"
#include <qjsonarray.h>
#include <qjsonvalue.h>

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

Keyboard::Shortcut ActionPannelParser::parseKeyboardShortcut(const QJsonValue &shortcut) {
  if (shortcut.isString()) {
    auto str = shortcut.toString();

    if (auto it = NAMED_SHORTCUT_MAP.find(str); it != NAMED_SHORTCUT_MAP.end()) { return it->second; }
    return Keyboard::Shortcut::fromString(shortcut.toString());
  }

  auto obj = shortcut.toObject();
  auto key = Keyboard::keyFromString(obj.value("key").toString());
  if (!key) return {};

  Qt::KeyboardModifiers modifiers{};
  for (const auto &modifierValue : obj.value("modifiers").toArray()) {
    auto modifier = Keyboard::modifierFromString(modifierValue.toString());
    if (!modifier) return {};
    modifiers.setFlag(*modifier);
  }

  return Keyboard::Shortcut(*key, modifiers);
}
