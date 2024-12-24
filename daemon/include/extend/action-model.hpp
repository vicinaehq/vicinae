#pragma once
#include "extend/image-model.hpp"
#include <qjsonobject.h>

struct ActionModel {
  QString title;
  QString onAction;
  std::optional<ImageLikeModel> icon;
};

struct ActionPannelSectionModel {
  QList<ActionModel> actions;
};

struct ActionPannelSubmenuModel {
  QString title;
  std::optional<ImageLikeModel> icon;
  QString onOpen;
  QString onSearchTextChange;
  QList<std::variant<ActionPannelSectionModel, ActionModel>> children;
};

using ActionPannelItem = std::variant<ActionModel, ActionPannelSectionModel,
                                      ActionPannelSubmenuModel>;

struct ActionPannelModel {
  QString title;
  QList<ActionPannelItem> children;
};

class ActionPannelParser {
  ActionModel parseAction(const QJsonObject &instance);

  ActionPannelSectionModel
  parseActionPannelSection(const QJsonObject &instance);
  ActionPannelSubmenuModel
  parseActionPannelSubmenu(const QJsonObject &instance);

public:
  ActionPannelParser();

  ActionPannelModel parse(const QJsonObject &instance);
};
