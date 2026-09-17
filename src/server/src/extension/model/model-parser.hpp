#pragma once
#include <QString>
#include <variant>
#include <vector>
#include "extension/model/form-model.hpp"
#include "extension/model/grid-model.hpp"
#include "extension/model/list-model.hpp"
#include "extension/model/root-detail-model.hpp"

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
