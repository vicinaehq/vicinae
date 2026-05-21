#pragma once
#include "extend/tag-model.hpp"
#include "ui/image/url.hpp"
#include <optional>
#include <string>
#include <variant>
#include <vector>

struct MetadataLabel {
  std::string text;
  std::string title;
  std::optional<ImageURL> icon;
  std::optional<ColorLike> color;
};

struct MetadataLink {
  std::string title;
  std::string text;
  std::string target;
};

struct MetadataSeparator {};

using MetadataItem = std::variant<MetadataLabel, MetadataLink, MetadataSeparator, TagListModel>;

struct MetadataModel {
  std::vector<MetadataItem> children;
};
