#pragma once
#include "extend/action-model.hpp"
#include "extend/empty-view-model.hpp"
#include "extend/image-model.hpp"
#include "extend/pagination-model.hpp"
#include <qjsonobject.h>

struct GridItemViewModel {
  QString id;
  QString title;
  QString subtitle;
  ImageLikeModel content;
  std::optional<ActionPannelModel> actionPannel;
};

struct GridSectionModel {
  QString title;
  QString subtitle;
  QList<GridItemViewModel> children;
};

enum GridFit { GridContain, GridFill };

using GridChild = std::variant<GridItemViewModel, GridSectionModel>;

struct GridModel {
  bool isLoading;
  bool filtering;
  bool throttle;
  double aspectRatio;
  int columns;
  int inset;
  GridFit fit;

  QString navigationTitle;
  QString searchPlaceholderText;
  std::optional<QString> onSelectionChanged;
  std::optional<QString> onSearchTextChange;
  std::optional<QString> searchText;
  std::vector<GridChild> items;
  std::optional<ActionPannelModel> actions;
  std::optional<EmptyViewModel> emptyView;
  std::optional<QString> selectedItemId;
  std::optional<PaginationModel> pagination;

  // not implemented, placeholder for now
  std::optional<int> searchBarAccessory;
};

class GridModelParser {
  GridItemViewModel parseListItem(const QJsonObject &instance, size_t index);
  GridSectionModel parseSection(const QJsonObject &instance);

public:
  GridModelParser();

  GridModel parse(const QJsonObject &instance);
};
