#include "extend/detail-model.hpp"
#include "extend/metadata-model.hpp"
#include "extend/node-props.hpp"

DetailModel DetailModelParser::parse(const Node &node, const NodeTree &tree) {
  DetailModel detail;
  const auto &props = node.props;

  if (auto sv = node_props::getString(props, "markdown")) {
    detail.markdown = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
  }

  forEachChild(node, tree, [&](const Node &child) {
    if (child.type == "metadata") { detail.metadata = MetadataModelParser().parse(child, tree); }
  });

  return detail;
}
