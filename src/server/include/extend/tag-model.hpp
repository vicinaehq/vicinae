#pragma once
#include "extend/color-model.hpp"
#include "extend/image-model.hpp"
#include "extend/node-tree.hpp"
#include <qstring.h>

struct TagItemModel {
  QString text;
  std::optional<ImageLikeModel> icon;
  std::optional<ColorLike> color;
  QString onAction;
};

struct TagListModel {
  QString title;
  QList<TagItemModel> items;
};

class TagListParser {
  TagItemModel parseTagItem(const Node &node);

public:
  TagListModel parse(const Node &node, const NodeTree &tree);
};
