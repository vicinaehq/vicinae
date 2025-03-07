#pragma once

#include "extend/action-model.hpp"
#include "extend/image-model.hpp"
#include <optional>
#include <qjsonobject.h>

struct EmptyViewModel {
  QString title;
  QString description;
  std::optional<ImageLikeModel> icon;
  std::optional<ActionPannelModel> actions;
};

class EmptyViewModelParser {
public:
  EmptyViewModelParser();
  EmptyViewModel parse(const QJsonObject &instance);
};
