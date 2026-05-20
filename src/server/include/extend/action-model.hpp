#pragma once
#include <QJsonObject>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include "extend/image-model.hpp"
#include "lib/keyboard/keyboard.hpp"

struct ActionModel {
  std::string title;
  std::string onAction;
  std::optional<std::string> onSubmit;
  std::optional<ImageLikeModel> icon;
  std::optional<Keyboard::Shortcut> shortcut;
  std::string type = "callback";
  QJsonObject quicklink;
  std::optional<std::string> stableId;
};

struct ActionPannelSectionModel;
struct ActionPannelSubmenuModel;

using ActionPannelSectionPtr = std::shared_ptr<ActionPannelSectionModel>;
using ActionPannelSubmenuPtr = std::shared_ptr<ActionPannelSubmenuModel>;
using ActionPannelSectionItem = std::variant<ActionModel, ActionPannelSubmenuPtr>;
using ActionPannelSubmenuChild = std::variant<ActionPannelSectionPtr, ActionModel, ActionPannelSubmenuPtr>;

struct ActionPannelSubmenuFiltering {
  bool keepSectionOrder = false;
};

struct ActionPannelSectionModel {
  std::string title;
  std::vector<ActionPannelSectionItem> items;

  std::vector<ActionModel> actions() const {
    std::vector<ActionModel> result;
    for (const auto &item : items) {
      if (auto action = std::get_if<ActionModel>(&item)) { result.push_back(*action); }
    }
    return result;
  }
};

struct ActionPannelSubmenuModel {
  std::string title;
  std::optional<ImageLikeModel> icon;
  std::optional<Keyboard::Shortcut> shortcut;
  std::optional<bool> autoFocus;
  std::optional<std::variant<bool, ActionPannelSubmenuFiltering>> filtering;
  std::optional<bool> isLoading;
  std::optional<bool> throttle;
  std::string onOpen;
  std::string onSearchTextChange;
  std::vector<ActionPannelSubmenuChild> children;
  std::optional<std::string> stableId;
};

using ActionPannelItem = std::variant<ActionModel, ActionPannelSectionPtr, ActionPannelSubmenuPtr>;

struct ActionPannelModel {
  bool dirty = true;
  std::string title;
  std::vector<ActionPannelItem> children;
  std::optional<std::string> stableId;
};
