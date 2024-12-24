#pragma once
#include "extend/action-model.hpp"
#include "extend/detail-model.hpp"
#include "extend/image-model.hpp"
#include <qjsonobject.h>

struct ListItemViewModel {
  bool changed;
  QString id;
  QString title;
  QString subtitle;
  ImageLikeModel icon;
  std::optional<DetailModel> detail;
  std::optional<ActionPannelModel> actionPannel;
};

struct ListModel {
  bool isLoading;
  bool isFiltering;
  bool isShowingDetail;
  QString navigationTitle;
  QString searchPlaceholderText;
  QString onSearchTextChange;
  QList<ListItemViewModel> items;
};

class ListModelParser {
  ListItemViewModel parseListItem(const QJsonObject &instance);

public:
  ListModelParser();

  ListModel parse(const QJsonObject &instance);
};
