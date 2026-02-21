#include "view-utils.hpp"
#include "ui/omni-painter/omni-painter.hpp"

namespace qml {

QVariantList metadataToVariantList(const MetadataModel &metadata) {
  QVariantList result;
  for (const auto &child : metadata.children) {
    if (auto *label = std::get_if<MetadataLabel>(&child)) {
      QVariantMap entry;
      entry[QStringLiteral("type")] = QStringLiteral("label");
      entry[QStringLiteral("label")] = label->title;
      entry[QStringLiteral("value")] = label->text;
      if (label->icon) entry[QStringLiteral("icon")] = imageSourceFor(ImageURL(*label->icon));
      if (label->color)
        entry[QStringLiteral("valueColor")] = OmniPainter::resolveColor(*label->color).name();
      result.append(entry);
    } else if (auto *link = std::get_if<MetadataLink>(&child)) {
      QVariantMap entry;
      entry[QStringLiteral("type")] = QStringLiteral("link");
      entry[QStringLiteral("label")] = link->title;
      entry[QStringLiteral("value")] = link->text;
      entry[QStringLiteral("url")] = link->target;
      result.append(entry);
    } else if (std::get_if<MetadataSeparator>(&child)) {
      QVariantMap entry;
      entry[QStringLiteral("type")] = QStringLiteral("separator");
      result.append(entry);
    } else if (auto *tags = std::get_if<TagListModel>(&child)) {
      QVariantMap entry;
      entry[QStringLiteral("type")] = QStringLiteral("tags");
      entry[QStringLiteral("label")] = tags->title;
      QVariantList tagList;
      for (const auto &tag : tags->items) {
        QVariantMap t;
        t[QStringLiteral("text")] = tag.text;
        if (tag.color) t[QStringLiteral("color")] = OmniPainter::resolveColor(*tag.color).name();
        if (tag.icon) t[QStringLiteral("icon")] = imageSourceFor(ImageURL(*tag.icon));
        tagList.append(t);
      }
      entry[QStringLiteral("tags")] = tagList;
      result.append(entry);
    }
  }
  return result;
}

} // namespace qml
