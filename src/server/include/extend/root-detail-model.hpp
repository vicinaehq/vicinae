#pragma once

#include "extend/action-model.hpp"
#include "extend/metadata-model.hpp"
#include <qjsonobject.h>

struct RootDetailModel {
  bool isLoading;
  QString markdown;
  std::optional<MetadataModel> metadata;
  std::optional<ActionPannelModel> actions;
  std::optional<QString> navigationTitle;
};

class RootDetailModelParser {
public:
  RootDetailModelParser();
  RootDetailModel parse(const QJsonObject &instance);
};
