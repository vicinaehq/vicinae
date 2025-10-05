#include "common.hpp"
#include "keybind-manager.hpp"
#include "keyboard/keyboard.hpp"
#include <qjsonobject.h>
#include <qnamespace.h>
#include <qobject.h>
#include <QString>
#include <qtmetamacros.h>
#include <unordered_map>
#include <vector>

// clang-format off
static const std::unordered_map<Keybind, KeybindInfo> infos{
	{Keybind::ToggleActionPanel, KeybindInfo{
		.id = "toggle-action-panel", 
		.name = "Toggle action panel",
		.description = "Toggle the action panel to access and filter through the list of available actions for the currently selected item",
		.icon = "switch",
		.dflt = Keyboard::Shortcut(Qt::Key_B, Qt::ControlModifier)
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
	{Keybind::NewAction, KeybindInfo{
			.id = "action.new",
			.name = "Generic New Action",
			.description = "Can be used by actions that create or duplicate something",
			 .icon = "new-document",
			.dflt = Keyboard::Shortcut(Qt::Key_N, Qt::ControlModifier)
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
