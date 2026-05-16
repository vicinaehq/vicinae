#pragma once
#include "extend/accessory-model.hpp"
#include "extend/action-model.hpp"
#include "extend/detail-model.hpp"
#include "extend/empty-view-model.hpp"
#include "extend/event-counted.hpp"
#include "extend/image-model.hpp"
#include "extend/dropdown-model.hpp"
#include "extend/node-tree.hpp"
#include "extend/pagination-model.hpp"

struct ListItemViewModel {
  std::string id;
  std::string title;
  std::string subtitle;
  std::optional<ImageLikeModel> icon;
  std::optional<DetailModel> detail;
  std::optional<ActionPannelModel> actionPannel;
  std::vector<AccessoryModel> accessories;
  std::vector<std::string> keywords;
};

struct ListSectionModel {
  std::string title;
  std::string subtitle;
  std::vector<ListItemViewModel> children;
};

using ListChild = std::variant<ListItemViewModel, ListSectionModel>;

using ListSearchBarAccessory = std::variant<DropdownModel>;

struct ListModel {
  bool dirty = true;
  bool isLoading = false;
  bool filtering = false;
  bool throttle = false;
  bool isShowingDetail = false;
  std::string navigationTitle;
  std::string searchPlaceholderText;
  std::optional<std::string> onSelectionChanged;
  std::optional<std::string> onSearchTextChange;
  std::optional<EventCounted<std::string>> searchText;
  std::vector<ListChild> items;
  std::optional<ActionPannelModel> actions;
  std::optional<EmptyViewModel> emptyView;
  std::optional<PaginationModel> pagination;
  std::optional<ListSearchBarAccessory> searchBarAccessory;
};

class ListModelParser {
  ListItemViewModel parseListItem(const Node &node, const NodeTree &tree, size_t index);
  ListSectionModel parseSection(const Node &node, const NodeTree &tree);
  ImageLikeModel parseListItemIcon(const glz::generic &value) const;
  std::string parseListItemTitle(const glz::generic &value) const;

public:
  ListModel parse(const Node &node, const NodeTree &tree);
};
