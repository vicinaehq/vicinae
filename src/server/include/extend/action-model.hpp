#pragma once
#include <QJsonArray>
#include <QList>
#include <QJsonValue>
#include <QStringList>
#include <memory>
#include <optional>
#include <qjsonobject.h>
#include <qnamespace.h>
#include <unordered_map>
#include <variant>
#include <vector>
#include "extend/image-model.hpp"
#include "lib/keyboard/keyboard.hpp"

struct KeyboardShortcutModel {
  QString key;
  QStringList modifiers;

  static KeyboardShortcutModel submit() { return {.key = "return", .modifiers = {"shift"}}; }
  static KeyboardShortcutModel remove() { return {.key = "X", .modifiers = {"ctrl"}}; }
  static KeyboardShortcutModel edit() { return {.key = "E", .modifiers = {"ctrl"}}; }
  static KeyboardShortcutModel removeAll() { return {.key = "X", .modifiers = {"ctrl", "shift"}}; }
  static KeyboardShortcutModel open() { return {.key = "O", .modifiers = {"ctrl"}}; }
  static KeyboardShortcutModel enter() { return {.key = "return"}; }

  bool operator==(const KeyboardShortcutModel &rhs) const {
    return key == rhs.key && modifiers == rhs.modifiers;
  }
};

struct ActionModel {
  QString title;
  QString onAction;
  std::optional<QString> onSubmit;
  std::optional<ImageLikeModel> icon;
  std::optional<Keyboard::Shortcut> shortcut;
  QString type;
  QJsonObject quicklink;
  std::optional<QString> stableId;
};

struct ActionPannelSectionModel;
struct ActionPannelSubmenuModel;

using ActionPannelSectionPtr = std::shared_ptr<ActionPannelSectionModel>;
using ActionPannelSubmenuPtr = std::shared_ptr<ActionPannelSubmenuModel>;
using ActionPannelSectionItem = std::variant<ActionModel, ActionPannelSubmenuPtr>;
using ActionPannelSubmenuChild = std::variant<ActionPannelSectionPtr, ActionModel, ActionPannelSubmenuPtr>;

struct ActionPannelSectionModel {
  QString title;
  QList<ActionPannelSectionItem> items;

  QList<ActionModel> actions() const {
    QList<ActionModel> result;
    for (const auto &item : items) {
      if (auto action = std::get_if<ActionModel>(&item)) { result.push_back(*action); }
    }
    return result;
  }
};

struct ActionPannelSubmenuFiltering {
  bool keepSectionOrder = false;
};

struct ActionPannelSubmenuModel {
  QString title;
  std::optional<ImageLikeModel> icon;
  std::optional<Keyboard::Shortcut> shortcut;
  std::optional<bool> autoFocus;
  std::optional<std::variant<bool, ActionPannelSubmenuFiltering>> filtering;
  std::optional<bool> isLoading;
  std::optional<bool> throttle;
  QString onOpen;
  QString onSearchTextChange;
  QList<ActionPannelSubmenuChild> children;
  std::optional<QString> stableId;
};

using ActionPannelItem = std::variant<ActionModel, ActionPannelSectionPtr, ActionPannelSubmenuPtr>;

struct ActionPannelModel {
  bool dirty;
  QString title;
  std::vector<ActionPannelItem> children;
  std::optional<QString> stableId;
};

static const std::unordered_map<QString, Keybind> NAMED_SHORTCUT_MAP = {
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

class ActionPannelParser {
  ActionModel parseAction(const QJsonObject &instance);

  ActionPannelSectionPtr parseActionPannelSection(const QJsonObject &instance);
  ActionPannelSubmenuPtr parseActionPannelSubmenu(const QJsonObject &instance);

public:
  ActionPannelParser();
  static Keyboard::Shortcut parseKeyboardShortcut(const QJsonValue &shortcut) {
    if (shortcut.isString()) {
      auto str = shortcut.toString();

      if (auto it = NAMED_SHORTCUT_MAP.find(str); it != NAMED_SHORTCUT_MAP.end()) { return it->second; }
      return Keyboard::Shortcut::fromString(shortcut.toString());
    }

    auto obj = shortcut.toObject();
    auto key = Keyboard::keyFromString(obj.value("key").toString());
    if (!key) return {};

    Qt::KeyboardModifiers modifiers;
    for (const auto &modifierValue : obj.value("modifiers").toArray()) {
      auto modifier = Keyboard::modifierFromString(modifierValue.toString());
      if (!modifier) return {};
      modifiers.setFlag(*modifier);
    }

    return Keyboard::Shortcut(*key, modifiers);
  }

  ActionPannelModel parse(const QJsonObject &instance);
};
