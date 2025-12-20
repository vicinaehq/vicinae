#include "keybind-manager.hpp"

// clang-format off
static const std::unordered_map<Keybind, KeybindInfo> infos{
	{Keybind::ToggleActionPanel, KeybindInfo{
		.id = "toggle-action-panel", 
		.name = "Toggle action panel",
		.description = "Toggle the action panel to access and filter through the list of available actions for the currently selected item",
		.icon = "switch",
		.dflt = Keyboard::Shortcut(Qt::Key_B, Qt::ControlModifier)
	}},
	{Keybind::OpenSearchAccessorySelector, KeybindInfo{
		.id = "open-search-filter", 
		.name = "Open Search Filter",
		.description = "Open the search filter selector if present",
		.icon = "arrow-up",
		.dflt = Keyboard::Shortcut(Qt::Key_P, Qt::ControlModifier)
	}},
	{Keybind::OpenSettings, KeybindInfo{
		.id = "open-settings", 
		.name = "Open settings window",
		.description = "Open this settings window from the launcher window",
		.icon = "cog",
		.dflt = Keyboard::Shortcut(Qt::Key_Comma, Qt::ControlModifier)
	}},
	{Keybind::OpenAction, KeybindInfo{
		.id = "action.open", 
		.name = "Generic Open Action",
		.description = "Can be used by actions that can open the selected item",
		.icon = "arrow-up",
		.dflt = Keyboard::Shortcut(Qt::Key_O, Qt::ControlModifier)
	}},
	{Keybind::CopyAction, KeybindInfo{
		.id = "action.copy", 
		.name = "Generic Copy Action",
		.description = "Can be used by actions that can copy the selected item",
		.icon = "copy-clipboard",
		.dflt = Keyboard::Shortcut(Qt::Key_C, Qt::ControlModifier | Qt::ShiftModifier)
	}},
	{Keybind::CopyNameAction, KeybindInfo{
		.id = "action.copy-name", 
		.name = "Copy Name Action",
		.description = "Can be used by actions that can copy the name of the selected item",
		.icon = "copy-clipboard",
		.dflt = Keyboard::Shortcut(Qt::Key_Period, Qt::ControlModifier | Qt::ShiftModifier)
	}},
	{Keybind::CopyPathAction, KeybindInfo{
		.id = "action.copy-path", 
		.name = "Copy Path Action",
		.description = "Can be used by actions that can copy the path of the selected item",
		.icon = "copy-clipboard",
		.dflt = Keyboard::Shortcut(Qt::Key_Comma, Qt::ControlModifier | Qt::ShiftModifier)
	}},
	{Keybind::SaveAction, KeybindInfo{
		.id = "action.save", 
		.name = "Save Action",
		.description = "Can be used by actions that can save the selected item",
		.icon = "save-document",
		.dflt = Keyboard::Shortcut(Qt::Key_S, Qt::ControlModifier)
	}},
	{Keybind::DuplicateAction, KeybindInfo{
		.id = "action.duplicate", 
		.name = "Duplicate Action",
		.description = "Can be used by actions that can duplicate the selected item",
		.icon = "duplicate",
		.dflt = Keyboard::Shortcut(Qt::Key_D, Qt::ControlModifier)
	}},
	{Keybind::NewAction, KeybindInfo{
			.id = "action.new",
			.name = "Generic New Action",
			.description = "Can be used by actions that create something",
			 .icon = "new-document",
			.dflt = Keyboard::Shortcut(Qt::Key_N, Qt::ControlModifier)
	}},
	{Keybind::MoveUpAction, KeybindInfo{
			.id = "action.move-up",
			.name = "Generic Move Up Action",
			.description = "Can be used by actions that can move up the selected item. This does not affect list navigation controls.",
			 .icon = "arrow-up",
			.dflt = Keyboard::Shortcut(Qt::Key_Up, Qt::ControlModifier | Qt::ShiftModifier)
	}},
	{Keybind::MoveDownAction, KeybindInfo{
			.id = "action.move-down",
			.name = "Generic Move Down Action",
			.description = "Can be used by actions that can move down the selected item. This does not affect list navigation controls.",
			 .icon = "arrow-down",
			.dflt = Keyboard::Shortcut(Qt::Key_Down, Qt::ControlModifier | Qt::ShiftModifier)
	}},
	{Keybind::RefreshAction, KeybindInfo{
			.id = "action.refresh",
			.name = "Generic Refresh Action",
			.description = "Can be used by actions that can refresh the selected item",
			 .icon = "arrow-clockwise",
			.dflt = Keyboard::Shortcut(Qt::Key_R, Qt::ControlModifier)
	}},
	{Keybind::PinAction, KeybindInfo{
		.id = "action.pin",
		.name = "Generic Pin Action",
		.description = "Can be used by actions that can pin the selected item",
		.icon = "pin",
		.dflt = Keyboard::Shortcut(Qt::Key_P, Qt::ControlModifier | Qt::ShiftModifier)
	}},
	{Keybind::RemoveAction, KeybindInfo{
		.id = "action.remove",
		.name = "Remove Action",
		.description = "Can be used by actions that can remove the selected item. This is normally used for small, not too impactful removals.",
		.icon = "trash",
		.dflt = Keyboard::Shortcut(Qt::Key_X, Qt::ControlModifier)
	}},
	{Keybind::DangerousRemoveAction, KeybindInfo{
		.id = "action.dangerous-remove", 
		.name = "Dangerous Remove Action",
		.description = "Can be used by actions that perform an impactful removal, generally accompanied by a confirmation dialog.",
		.icon = "trash",
		.dflt = Keyboard::Shortcut(Qt::Key_X, Qt::ControlModifier | Qt::ShiftModifier)
	}},
	{Keybind::EditAction, KeybindInfo{
		.id = "action.edit", 
		.name = "Edit Action",
		.description = "Can be used by actions that can edit the currently selected item",
		.icon = "pencil",
		.dflt = Keyboard::Shortcut(Qt::Key_E, Qt::ControlModifier)
	}},
	{Keybind::EditSecondaryAction, KeybindInfo{
		.id = "action.edit-secondary", 
		.name = "Edit Secondary Action",
		.description = "Can be used by actions that can edit a secondary characteristic of the currently selected item",
		.icon = "pencil",
		.dflt = Keyboard::Shortcut(Qt::Key_E, Qt::ControlModifier | Qt::ShiftModifier)
	}}
};
// clang-format on

KeybindManager *KeybindManager::instance() {
  static KeybindManager manager;
  return &manager;
}

KeybindManager::KeybindManager() {
  for (const auto &[bind, info] : infos) {
    m_shortcuts[bind] = info.dflt;
    m_idToBind[info.id] = bind;
  }
}

Keyboard::Shortcut KeybindManager::resolve(Keybind bind) const {
  if (auto it = m_shortcuts.find(bind); it != m_shortcuts.end()) { return it->second; }
  if (auto it = infos.find(bind); it != infos.end()) { return it->second.dflt; }

  return Keyboard::Shortcut();
}

bool KeybindManager::isBound(Keybind bind) const { return m_shortcuts.contains(bind); }

bool KeybindManager::isBound(const Keyboard::Shortcut &shortcut) const {
  for (const auto &[bind, stcut] : m_shortcuts) {
    if (stcut == shortcut) return true;
  }
  return false;
}

/**
 * Get info about the keybind this shortcut is bound to, if any.
 */
std::optional<KeybindInfo> KeybindManager::findBoundInfo(const Keyboard::Shortcut &shortcut) const {
  for (const auto &[bind, stcut] : m_shortcuts) {
    if (stcut == shortcut) {
      if (auto it = infos.find(bind); it != infos.end()) { return it->second; }
    }
  }

  return {};
}

void KeybindManager::setKeybind(Keybind bind, const Keyboard::Shortcut &shortcut) {
  m_shortcuts[bind] = shortcut;
  emit keybindChanged(bind, shortcut);
}

void KeybindManager::mergeBinds(const SerializedKeybindMap &map) {
  for (const auto &[bind, info] : infos) {
    if (auto it = map.find(info.id.toStdString()); it != map.end()) {
      m_shortcuts[bind] = Keyboard::Shortcut{it->second.c_str()};
    } else {
      m_shortcuts[bind] = info.dflt;
    }
  }
}

void KeybindManager::setMap(const KeybindMap &map) { m_shortcuts = map; }

const KeybindManager::KeybindMap &KeybindManager::map() const { return m_shortcuts; }

std::vector<std::pair<Keybind, const KeybindInfo *>> KeybindManager::orderedInfoList() {
  std::vector<std::pair<Keybind, const KeybindInfo *>> list;
  list.reserve(static_cast<uint8_t>(Keybind::KeybindEnd));

  for (uint8_t i = 0; i != static_cast<uint8_t>(Keybind::KeybindEnd); ++i) {
    auto bind = static_cast<Keybind>(i);
    if (auto it = infos.find(bind); it != infos.end()) { list.push_back({bind, &it->second}); }
  }

  return list;
}

KeybindManager::SerializedKeybindMap KeybindManager::toSerializedMap() {
  SerializedKeybindMap mp;

  for (const auto &[bind, shortcut] : m_shortcuts) {
    auto it = infos.find(bind);
    if (it == infos.end()) { continue; }
    auto &info = it->second;
    mp[info.id.toStdString()] = shortcut.toString().toStdString();
  }

  return mp;
}
