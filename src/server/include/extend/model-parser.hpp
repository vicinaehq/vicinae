#pragma once
#include "extend/form-model.hpp"
#include "extend/grid-model.hpp"
#include "extend/list-model.hpp"
#include "extend/node-tree.hpp"
#include "extend/root-detail-model.hpp"

struct InvalidModel {
  QString error;
};

using RenderModel = std::variant<ListModel, GridModel, FormModel, RootDetailModel, InvalidModel>;

struct RenderRoot {
  bool dirty;
  bool propsDirty;
  RenderModel root;
};

struct ParsedRenderData {
  std::vector<RenderRoot> items;
};

class ModelParser {
public:
  ParsedRenderData parse(const NodeTree &tree, const ApplyResult &result);
};
