#include "extend/model-parser.hpp"
#include "extend/list-model.hpp"
#include "extend/root-detail-model.hpp"
#include <qjsonobject.h>

ModelParser::ModelParser() {}

RenderModel ModelParser::parse(const QJsonObject &instance) {
  auto type = instance.value("type").toString();

  if (type == "list") {
    return ListModelParser().parse(instance);
  }

  if (type == "detail") {
    return RootDetailModelParser().parse(instance);
  }

  return InvalidModel{
      QString("Component of type %1 cannot be used as the root").arg(type)};
}
