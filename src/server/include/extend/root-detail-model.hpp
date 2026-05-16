#pragma once

#include "extend/action-model.hpp"
#include "extend/metadata-model.hpp"
#include "extend/node-tree.hpp"

struct RootDetailModel {
  bool isLoading;
  QString markdown;
  std::optional<MetadataModel> metadata;
  std::optional<ActionPannelModel> actions;
  std::optional<QString> navigationTitle;
};

class RootDetailModelParser {
public:
  RootDetailModel parse(const Node &node, const NodeTree &tree);
};
