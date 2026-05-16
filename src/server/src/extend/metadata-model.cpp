#include "extend/metadata-model.hpp"
#include "extend/color-model.hpp"
#include "extend/image-model.hpp"
#include "extend/node-props.hpp"
#include "extend/tag-model.hpp"

MetadataModel MetadataModelParser::parse(const Node &node, const NodeTree &tree) {
  std::vector<MetadataItem> items;
  items.reserve(node.childIds.size());

  forEachChild(node, tree, [&](const Node &child) {
    const auto &props = child.props;

    if (child.type == "metadata-label") {
      MetadataLabel label{
          .title = QString::fromStdString(node_props::getStringOr(props, "title")),
      };

      if (auto *textValue = node_props::get(props, "text")) {
        if (textValue->is_string()) {
          label.text = QString::fromStdString(textValue->get_string());
        } else if (textValue->is_object()) {
          const auto &textObj = textValue->get_object();
          if (textObj.contains("value") && textObj.at("value").is_string()) {
            label.text = QString::fromStdString(textObj.at("value").get_string());
          }
          if (textObj.contains("color")) { label.color = ColorLikeModelParser().parse(textObj.at("color")); }
        }
      }

      if (auto *v = node_props::get(props, "icon")) { label.icon = ImageModelParser().parse(*v); }

      items.emplace_back(label);
    }

    else if (child.type == "metadata-link") {
      items.emplace_back(MetadataLink{
          .title = QString::fromStdString(node_props::getStringOr(props, "title")),
          .text = QString::fromStdString(node_props::getStringOr(props, "text")),
          .target = QString::fromStdString(node_props::getStringOr(props, "target")),
      });
    }

    else if (child.type == "metadata-separator") {
      items.emplace_back(MetadataSeparator{});
    }

    else if (child.type == "tag-list") {
      items.emplace_back(TagListParser().parse(child, tree));
    }
  });

  return {items};
}
