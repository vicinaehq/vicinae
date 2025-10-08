#include "extend/metadata-model.hpp"
#include "extend/image-model.hpp"
#include "extend/tag-model.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>

MetadataModelParser::MetadataModelParser() {}

MetadataModel MetadataModelParser::parse(const QJsonObject &instance) {
  auto children = instance["children"].toArray();
  std::vector<MetadataItem> items;

  items.reserve(children.size());
  for (const auto &ref : children) {
    auto child = ref.toObject();
    auto type = child["type"].toString();
    auto props = child["props"].toObject();

    if (type == "metadata-label") {
      MetadataLabel label{
          .text = props["text"].toString(),
          .title = props["title"].toString(),
      };

      if (props.contains("icon")) { label.icon = ImageModelParser().parse(props.value("icon")); }

      items.emplace_back(label);
    }

    if (type == "metadata-link") {
      items.emplace_back(MetadataLink{
          .title = props.value("title").toString(),
          .text = props.value("text").toString(),
          .target = props.value("target").toString(),
      });
    }

    if (type == "metadata-separator") { items.push_back(MetadataSeparator{}); }

    if (type == "tag-list") { items.push_back(TagListParser().parse(child)); }
  }

  return {items};
}
