#include "extend/color-model.hpp"
#include "extend/image-model.hpp"
#include "extend/node-props.hpp"
#include "extend/tag-model.hpp"

TagItemModel TagListParser::parseTagItem(const Node &node) {
  TagItemModel model;
  const auto &props = node.props;

  if (auto *v = node_props::get(props, "icon")) model.icon = ImageModelParser().parse(*v);
  if (auto *v = node_props::get(props, "color")) { model.color = ColorLikeModelParser().parse(*v); }

  model.text = QString::fromStdString(node_props::getStringOr(props, "text"));
  model.onAction = QString::fromStdString(node_props::getStringOr(props, "onAction"));

  return model;
}

TagListModel TagListParser::parse(const Node &node, const NodeTree &tree) {
  TagListModel model;
  const auto &props = node.props;

  model.title = QString::fromStdString(node_props::getStringOr(props, "title"));

  forEachChild(node, tree, [&](const Node &child) { model.items.push_back(parseTagItem(child)); });

  return model;
}
