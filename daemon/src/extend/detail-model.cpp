#include "extend/detail-model.hpp"
#include "extend/metadata-model.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>

DetailModelParser::DetailModelParser() {}

DetailModel DetailModelParser::parse(const QJsonObject &instance) {
  DetailModel detail;
  auto props = instance.value("props").toObject();
  auto children = instance.value("children").toArray();

  detail.markdown = props["markdown"].toString();

  for (const auto &child : children) {
    auto obj = child.toObject();
    auto type = obj["type"].toString();

    if (type == "list-item-detail-metadata") {
      detail.metadata = MetadataModelParser().parse(obj);
    }
  }

  return detail;
}
