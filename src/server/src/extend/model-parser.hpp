#pragma once
#include <QString>
#include <variant>
#include <vector>
#include "extend/form-model.hpp"
#include "extend/grid-model.hpp"
#include "extend/list-model.hpp"
#include "extend/root-detail-model.hpp"

struct InvalidModel {
  QString error;
};

using RenderModel = std::variant<ListModel, GridModel, FormModel, RootDetailModel, InvalidModel>;

struct RenderRoot {
  bool dirty;
  RenderModel root;
};

struct ParsedRenderData {
  std::vector<RenderRoot> items;
};
