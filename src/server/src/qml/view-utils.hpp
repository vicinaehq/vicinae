#pragma once
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

} // namespace qml
