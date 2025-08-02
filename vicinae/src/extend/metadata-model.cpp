#include "extend/metadata-model.hpp"
#include "extend/tag-model.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>

MetadataModelParser::MetadataModelParser() {}

MetadataModel MetadataModelParser::parse(const QJsonObject &instance) {
  auto children = instance["children"].toArray();
  QList<MetadataItem> items;

  for (const auto &ref : children) {
    auto child = ref.toObject();
    auto type = child["type"].toString();
    auto props = child["props"].toObject();

    if (type == "metadata-label") {
      items.push_back(MetadataLabel{
          .text = props["text"].toString(),
          .title = props["title"].toString(),
      });
    }

    if (type == "metadata-link") {
      items.push_back(MetadataLink{
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
