#pragma once
#include "extend/image-model.hpp"
#include <optional>
#include <string>
#include <variant>
#include <vector>

struct DropdownModel {
  struct Item {
    std::string title;
    std::string value;
    std::optional<ImageLikeModel> icon;
    std::vector<std::string> keywords;
  };

  struct Section {
    std::string title;
    std::vector<Item> items;
  };

  using Child = std::variant<Item, Section>;

  struct Filtering {
    bool keepSectionOrder = true;
    bool enabled = true;
  };

  bool dirty = true;
  std::optional<std::string> tooltip;
  std::optional<std::string> defaultValue;
  std::optional<std::string> id;
  std::optional<std::string> onChange;
  std::optional<std::string> onSearchTextChange;
  std::optional<std::string> placeholder;
  std::optional<std::string> value;
  std::vector<Child> children;
  bool storeValue = true;
  bool throttle = false;
  Filtering filtering;
  bool isLoading = false;
};
