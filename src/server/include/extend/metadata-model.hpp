#pragma once
#include "extend/tag-model.hpp"
#include "../theme.hpp"
#include "../ui/image/url.hpp"
#include "extend/node-tree.hpp"

struct MetadataLabel {
  QString text;
  QString title;
  std::optional<ImageURL> icon;
  std::optional<ColorLike> color;
};

struct MetadataLink {
  QString title;
  QString text;
  QString target;
};

struct MetadataSeparator {};

using MetadataItem = std::variant<MetadataLabel, MetadataLink, MetadataSeparator, TagListModel>;

struct MetadataModel {
  std::vector<MetadataItem> children;
};

class MetadataModelParser {
public:
  MetadataModel parse(const Node &node, const NodeTree &tree);
};
