#include "extend/root-detail-model.hpp"
#include "extend/action-model.hpp"
#include "extend/metadata-model.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>

RootDetailModelParser::RootDetailModelParser() {}

RootDetailModel RootDetailModelParser::parse(const QJsonObject &instance) {
  RootDetailModel model;
  auto props = instance.value("props").toObject();

  if (props.contains("navigationTitle")) {
    model.navigationTitle = props.value("navigationTitle").toString();
  }

  model.markdown = props.value("markdown").toString();
  model.isLoading = props.value("isLoading").toBool();

  for (const auto &child : instance.value("children").toArray()) {
    auto obj = child.toObject();
    auto type = obj.value("type").toString();

    if (type == "action-panel") {
      model.actions = ActionPannelParser().parse(obj);
    }

    else if (type == "metadata") {
      model.metadata = MetadataModelParser().parse(obj);
    }
  }

  return model;
}
