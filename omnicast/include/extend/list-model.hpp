#pragma once
#include "extend/action-model.hpp"
#include "extend/detail-model.hpp"
#include "extend/empty-view-model.hpp"
#include "extend/image-model.hpp"
#include "extend/pagination-model.hpp"
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

struct ListSectionModel {
  QString title;
  QString subtitle;
  QList<ListItemViewModel> children;
};

using ListChild = std::variant<ListItemViewModel, ListSectionModel>;

struct ListModel {
  bool isLoading;
  bool filtering;
  bool throttle;
  bool isShowingDetail;
  QString navigationTitle;
  QString searchPlaceholderText;
  std::optional<QString> onSelectionChanged;
  std::optional<QString> onSearchTextChange;
  std::optional<QString> searchText;
  std::vector<ListChild> items;
  std::optional<ActionPannelModel> actions;
  std::optional<EmptyViewModel> emptyView;
  std::optional<QString> selectedItemId;
  std::optional<PaginationModel> pagination;

  // not implemented, placeholder for now
  std::optional<int> searchBarAccessory;
};

class ListModelParser {
  ListItemViewModel parseListItem(const QJsonObject &instance, size_t index);
  ListSectionModel parseSection(const QJsonObject &instance);

public:
  ListModelParser();

  ListModel parse(const QJsonObject &instance);
};
