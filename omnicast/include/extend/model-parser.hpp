#pragma once
#include "extend/grid-model.hpp"
#include "extend/list-model.hpp"
#include "extend/root-detail-model.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>

struct InvalidModel {
  QString error;
};

using RenderTree = QJsonObject;
using RenderModel = std::variant<ListModel, GridModel, RootDetailModel, InvalidModel>;

class ModelParser {
public:
  ModelParser();

  std::vector<RenderModel> parse(const QJsonArray &views);
};
