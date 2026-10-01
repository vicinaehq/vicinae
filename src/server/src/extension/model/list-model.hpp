#pragma once
#include "extension/model/accessory-model.hpp"
#include "extension/model/action-model.hpp"
#include "extension/model/detail-model.hpp"
#include "extension/model/empty-view-model.hpp"
#include "extension/model/event-counted.hpp"
#include "extension/model/image-model.hpp"
#include "extension/model/dropdown-model.hpp"
#include "extension/model/pagination-model.hpp"
#include "services/clipboard/clipboard-content.hpp"

struct ListItemViewModel {
  bool changed = false;
  std::string id;
  std::string title;
  std::string subtitle;
  std::optional<ImageLikeModel> icon;
  std::optional<DetailModel> detail;
  std::optional<ActionPannelModel> actionPannel;
  std::vector<AccessoryModel> accessories;
  std::vector<std::string> keywords;
  std::optional<Clipboard::Content> dragContent;
};

struct ListSectionModel {
  std::string title;
  std::string subtitle;
  std::vector<ListItemViewModel> children;
};

using ListChild = std::variant<ListItemViewModel, ListSectionModel>;

using ListSearchBarAccessory = std::variant<DropdownModel>;

struct ListModel {
  bool isLoading = false;
  bool filtering = false;
  bool throttle = false;
  bool dirty = false;
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
