#include "extend/empty-view-model.hpp"
#include "extend/action-model.hpp"
#include "extend/image-model.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>

EmptyViewModelParser::EmptyViewModelParser() {}

EmptyViewModel EmptyViewModelParser::parse(const QJsonObject &instance) {
  EmptyViewModel model;
  auto props = instance.value("props").toObject();

  model.title = props.value("title").toString();
  model.description = props.value("description").toString();

  if (props.contains("icon")) { model.icon = ImageModelParser().parse(props.value("icon")); }

  for (const auto &child : instance.value("children").toArray()) {
    auto obj = child.toObject();
    auto type = obj.value("type").toString();

    if (type == "action-panel") { model.actions = ActionPannelParser().parse(obj); }
  }

  return model;
}
