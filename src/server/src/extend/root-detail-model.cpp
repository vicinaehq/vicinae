#include "extend/root-detail-model.hpp"
#include "extend/action-model.hpp"
#include "extend/metadata-model.hpp"
#include "extend/node-props.hpp"

RootDetailModel RootDetailModelParser::parse(const Node &node, const NodeTree &tree) {
  RootDetailModel model;
  const auto &props = node.props;

  if (auto sv = node_props::getString(props, "navigationTitle")) {
    model.navigationTitle = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
  }

  model.markdown = QString::fromStdString(node_props::getStringOr(props, "markdown"));
  model.isLoading = node_props::getBool(props, "isLoading");

  forEachChild(node, tree, [&](const Node &child) {
    if (child.type == "action-panel") {
      model.actions = ActionPannelParser().parse(child, tree);
    } else if (child.type == "metadata") {
      model.metadata = MetadataModelParser().parse(child, tree);
    }
  });

  return model;
}
