#pragma once
#include "extend/dropdown-model.hpp"
#include "extend/metadata-model.hpp"
#include "image-url.hpp"
#include "ui/image/url.hpp"
#include <QString>
#include <QVariantList>

namespace qml {

QVariantList metadataToVariantList(const MetadataModel &metadata);

inline QString imageSourceFor(const ImageURL &url) {
  return ImageUrl(url).toSource();
}

inline std::optional<QString>
firstDropdownItemValue(const std::vector<DropdownModel::Child> &children) {
  for (const auto &child : children) {
    if (auto *item = std::get_if<DropdownModel::Item>(&child)) {
      return item->value;
    } else if (auto *section = std::get_if<DropdownModel::Section>(&child)) {
      if (!section->items.empty()) return section->items[0].value;
    }
  }
  return std::nullopt;
}

inline QVariantList convertDropdownChildren(const std::vector<DropdownModel::Child> &children) {
  QVariantList result;
  QVariantList unsectioned;

  for (const auto &child : children) {
    if (auto *item = std::get_if<DropdownModel::Item>(&child)) {
      QVariantMap m;
      m["id"] = item->value;
      m["displayName"] = item->title;
      m["iconSource"] = item->icon ? imageSourceFor(ImageURL(*item->icon)) : QString();
      unsectioned.append(m);
    } else if (auto *section = std::get_if<DropdownModel::Section>(&child)) {
      if (!unsectioned.isEmpty()) {
        QVariantMap defaultSection;
        defaultSection["title"] = QString();
        defaultSection["items"] = unsectioned;
        result.append(defaultSection);
        unsectioned.clear();
      }
      QVariantMap sectionMap;
      sectionMap["title"] = section->title;
      QVariantList sectionItems;
      for (const auto &si : section->items) {
        QVariantMap m;
        m["id"] = si.value;
        m["displayName"] = si.title;
        m["iconSource"] = si.icon ? imageSourceFor(ImageURL(*si.icon)) : QString();
        sectionItems.append(m);
      }
      sectionMap["items"] = sectionItems;
      result.append(sectionMap);
    }
  }

  if (!unsectioned.isEmpty()) {
    QVariantMap defaultSection;
    defaultSection["title"] = QString();
    defaultSection["items"] = unsectioned;
    result.append(defaultSection);
  }

  return result;
}

} // namespace qml
