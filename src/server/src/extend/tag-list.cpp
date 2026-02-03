#include "extend/color-model.hpp"
#include "extend/image-model.hpp"
#include "extend/tag-model.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>

TagListParser::TagListParser() {}

TagItemModel TagListParser::parseTagItem(const QJsonObject &instance) {
  TagItemModel model;
  auto props = instance.value("props").toObject();

  if (props.contains("icon")) model.icon = ImageModelParser().parse(props.value("icon"));

  if (props.contains("color")) { model.color = ColorLikeModelParser().parse(props.value("color")); }

  model.text = props.value("text").toString();
  model.onAction = props.value("onAction").toString();

  return model;
}

TagListModel TagListParser::parse(const QJsonObject &instance) {
  TagListModel model;
  auto props = instance.value("props").toObject();

  model.title = props.value("title").toString();

  for (const auto &child : instance.value("children").toArray()) {
    model.items.push_back(parseTagItem(child.toObject()));
  }

  return model;
}
