#include "extend/root-detail-model.hpp"
#include "extend/action-model.hpp"
#include "extend/metadata-model.hpp"
#include <qjsonobject.h>

RootDetailModelParser::RootDetailModelParser() {}

RootDetailModel RootDetailModelParser::parse(const QJsonObject &instance) {
  RootDetailModel model;

  model.navigationTitle = instance.value("navigationTitle").toString();
  model.markdown = instance.value("markdown").toString();
  model.isLoading = instance.value("isLoading").toBool();

  if (instance.contains("actions")) {
    model.actions =
        ActionPannelParser().parse(instance.value("actions").toObject());
  }

  if (instance.contains("metadata")) {
    model.metadata =
        MetadataModelParser().parse(instance.value("metadata").toObject());
  }

  return model;
}
