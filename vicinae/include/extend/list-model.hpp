#pragma once
#include "extend/accessory-model.hpp"
#include "extend/action-model.hpp"
#include "extend/detail-model.hpp"
#include "extend/empty-view-model.hpp"
#include "extend/image-model.hpp"
#include "extend/dropdown-model.hpp"
#include "extend/pagination-model.hpp"
#include <qjsonobject.h>

struct ListItemViewModel {
  bool changed;
  QString id;
  QString title;
  QString subtitle;
  std::optional<ImageLikeModel> icon;
  std::optional<DetailModel> detail;
  std::optional<ActionPannelModel> actionPannel;
  std::vector<AccessoryModel> accessories;
  std::vector<QString> keywords;
};

struct ListSectionModel {
  QString title;
  QString subtitle;
  std::vector<ListItemViewModel> children;
};

using ListChild = std::variant<ListItemViewModel, ListSectionModel>;

using ListSearchBarAccessory = std::variant<DropdownModel>;

struct ListModel {
  bool isLoading = false;
  bool filtering = false;
  bool throttle = false;
  bool dirty = false;
  bool propsDirty = false;
  bool isShowingDetail = false;
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
  std::optional<ListSearchBarAccessory> searchBarAccessory;
};

class ListModelParser {
  ListItemViewModel parseListItem(const QJsonObject &instance, size_t index);
  ListSectionModel parseSection(const QJsonObject &instance);
  ImageLikeModel parseListItemIcon(const QJsonValue &value) const;
  QString parseListItemTitle(const QJsonValue &value) const;

public:
  ListModelParser();

  ListModel parse(const QJsonObject &instance);
};
