#pragma once
#include "extend/list-model.hpp"
#include <qjsonobject.h>

struct InvalidModel {
  QString error;
};

using RenderTree = QJsonObject;
using RenderModel = std::variant<ListModel, InvalidModel>;

class ModelParser {
public:
  ModelParser();

  RenderModel parse(const QJsonObject &tree);
};
