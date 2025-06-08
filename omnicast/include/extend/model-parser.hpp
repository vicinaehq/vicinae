#pragma once
#include "extend/form-model.hpp"
#include "extend/grid-model.hpp"
#include "extend/list-model.hpp"
#include "extend/root-detail-model.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>

struct InvalidModel {
  QString error;
};

using RenderTree = QJsonObject;
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
  ModelParser();

  ParsedRenderData parse(const QJsonArray &views);
};
