#pragma once
#include "extend/metadata-model.hpp"
#include "qml-image-url.hpp"
#include "ui/image/url.hpp"
#include <QString>
#include <QVariantList>

namespace qml {

QVariantList metadataToVariantList(const MetadataModel &metadata);

inline QString imageSourceFor(const ImageURL &url) {
  return QmlImageUrl(url).toSource();
}

} // namespace qml
