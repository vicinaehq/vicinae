#pragma once
#include "common.hpp"
#include "keyboard/keyboard.hpp"
#include <qobject.h>
#include <QString>
#include <qtmetamacros.h>
#include <unordered_map>
#include <vector>

enum class Keybind {
  ToggleActionPanel,

  // common action keybinds
  OpenAction,
  CopyAction,
  PasteAction,
  NewAction,
  PinAction,

  KeybindEnd,
};

struct KeybindInfo {
  QString name;
};

// clang-format off
static const std::unordered_map<Keybind, KeybindInfo> infos{
	{Keybind::ToggleActionPanel, KeybindInfo{.name = "Toggle action panel"}},
	{Keybind::OpenAction, KeybindInfo{.name = "Generic Open Action"}},
	{Keybind::CopyAction, KeybindInfo{.name = "Generic Copy Action"}},
	{Keybind::PasteAction, KeybindInfo{.name = "Generic Paste Action"}},
	{Keybind::NewAction, KeybindInfo{.name = "Generic New Action"}},
	{Keybind::PinAction, KeybindInfo{.name = "Generic Pin Action"}}
};

// clang-format on

class KeybindManager : public QObject, NonCopyable {
  Q_OBJECT

signals:
  void keybindChanged(Keybind bind, const Keyboard::Shortcut &shortcut) const;

public:
  static KeybindManager *instance() {
    static KeybindManager manager;
    return &manager;
  }

  KeybindManager() { setKeybind(Keybind::CopyAction, Keyboard::Shortcut::copy()); }

  using KeybindMap = std::unordered_map<Keybind, Keyboard::Shortcut>;

  std::optional<Keyboard::Shortcut> resolve(Keybind bind) const {
    if (auto it = m_shortcuts.find(bind); it != m_shortcuts.end()) { return it->second; }
    return {};
  }

  auto list() const { return infos; }

  bool isBound(Keybind bind) const { return m_shortcuts.contains(bind); }
  bool isBound(const Keyboard::Shortcut &shortcut) const {
    for (const auto &[bind, stcut] : m_shortcuts) {
      if (stcut == shortcut) return true;
    }
    return false;
  }

  void setMap(const KeybindMap &map) { m_shortcuts = map; }
  const KeybindMap &map() const { return m_shortcuts; }

  void setKeybind(Keybind bind, const Keyboard::Shortcut &shortcut) {
    m_shortcuts[bind] = shortcut;
    emit keybindChanged(bind, shortcut);
  }

private:
  std::unordered_map<Keybind, Keyboard::Shortcut> m_shortcuts;
};
