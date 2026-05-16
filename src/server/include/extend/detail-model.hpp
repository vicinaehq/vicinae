#pragma once
#include "extend/metadata-model.hpp"
#include "extend/node-tree.hpp"

struct DetailModel {
  std::optional<QString> markdown;
  MetadataModel metadata;
};

class DetailModelParser {
public:
  DetailModel parse(const Node &node, const NodeTree &tree);
};
