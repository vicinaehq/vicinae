#pragma once
#include "extend/tag-model.hpp"
#include <qjsonobject.h>

struct MetadataLabel {
  QString text;
  QString title;
};

struct MetadataSeparator {};

using MetadataItem = std::variant<MetadataLabel, MetadataSeparator, TagListModel>;

struct MetadataModel {
  QList<MetadataItem> children;
};

class MetadataModelParser {
  MetadataItem parseMetadataItem(const QJsonObject &instance);

public:
  MetadataModelParser();

  MetadataModel parse(const QJsonObject &instance);
};
