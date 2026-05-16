#include "extend/empty-view-model.hpp"
#include "extend/action-model.hpp"
#include "extend/image-model.hpp"
#include "extend/node-props.hpp"

EmptyViewModel EmptyViewModelParser::parse(const Node &node, const NodeTree &tree) {
  EmptyViewModel model;
  const auto &props = node.props;

  model.title = QString::fromStdString(node_props::getStringOr(props, "title"));
  model.description = QString::fromStdString(node_props::getStringOr(props, "description"));

  if (auto *v = node_props::get(props, "icon")) { model.icon = ImageModelParser().parse(*v); }

  forEachChild(node, tree, [&](const Node &child) {
    if (child.type == "action-panel") { model.actions = ActionPannelParser().parse(child, tree); }
  });

  return model;
}
