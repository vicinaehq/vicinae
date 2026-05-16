#pragma once

#include "extend/action-model.hpp"
#include "extend/image-model.hpp"
#include "extend/node-tree.hpp"
#include <optional>

struct EmptyViewModel {
  QString title;
  QString description;
  std::optional<ImageLikeModel> icon;
  std::optional<ActionPannelModel> actions;
};

class EmptyViewModelParser {
public:
  EmptyViewModel parse(const Node &node, const NodeTree &tree);
};
