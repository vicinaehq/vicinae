#pragma once
#include "extend/metadata-model.hpp"
#include <qjsonobject.h>
#include <qjsonvalue.h>

struct DetailModel {
  std::optional<QString> markdown;
  MetadataModel metadata;
};

class DetailModelParser {
public:
  DetailModelParser();

  DetailModel parse(const QJsonObject &instance);
};
