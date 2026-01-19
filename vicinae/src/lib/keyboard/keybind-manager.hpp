#pragma once
#include "common/types.hpp"
#include "keyboard/keyboard.hpp"
#include <qjsonobject.h>
#include <qnamespace.h>
#include <qobject.h>
#include <QString>
#include <qtmetamacros.h>
#include <unordered_map>
#include <vector>
#include "keybind.hpp"

struct KeybindInfo {
  QString id;
  QString name;
  QString description;
  QString icon;
  Keyboard::Shortcut dflt;
};

class KeybindManager : public QObject, NonCopyable {
  Q_OBJECT

signals:
  void keybindChanged(Keybind bind, const Keyboard::Shortcut &shortcut) const;

public:
  static KeybindManager *instance();
  KeybindManager();

  using KeybindMap = std::unordered_map<Keybind, Keyboard::Shortcut>;
  // [keybind_id]: shortcut string
  using SerializedKeybindMap = std::unordered_map<std::string, std::string>;

  Keyboard::Shortcut resolve(Keybind bind) const;

  bool isBound(Keybind bind) const;
  bool isBound(const Keyboard::Shortcut &shortcut) const;

  /**
   * Get info about the keybind this shortcut is bound to, if any.
   */
  std::optional<KeybindInfo> findBoundInfo(const Keyboard::Shortcut &shortcut) const;

  void setMap(const KeybindMap &map);
  const KeybindMap &map() const;

  void setKeybind(Keybind bind, const Keyboard::Shortcut &shortcut);
  void mergeBinds(const SerializedKeybindMap &map);
  std::vector<std::pair<Keybind, const KeybindInfo *>> orderedInfoList();
  SerializedKeybindMap toSerializedMap();

private:
  std::unordered_map<Keybind, Keyboard::Shortcut> m_shortcuts;
  std::unordered_map<QString, Keybind> m_idToBind;
};
