#pragma once
#include "extend/action-model.hpp"
#include "extend/empty-view-model.hpp"
#include "extend/event-counted.hpp"
#include "extend/image-model.hpp"
#include "extend/pagination-model.hpp"
#include "extend/dropdown-model.hpp"
#include "ui/image/url.hpp"

enum GridFit { GridContain, GridFill };

enum class GridInset { None, Small, Medium, Large };

constexpr double insetRatio(GridInset inset) {
  switch (inset) {
  case GridInset::Small:
    return 0.10;
  case GridInset::Medium:
    return 0.15;
  case GridInset::Large:
    return 0.25;
  default:
    return 0.0;
  }
}

struct GridItemViewModel {
  using Content = std::variant<ImageLikeModel, ColorLike>;

  std::string id;
  std::string title;
  std::string subtitle;
  Content content;
  std::optional<std::string> tooltip;
  std::vector<std::string> keywords;
  std::optional<ActionPannelModel> actionPannel;
};

struct GridSectionModel {
  std::string title;
  std::string subtitle;

  std::optional<double> aspectRatio;
  std::optional<int> columns;
  std::optional<ObjectFit> fit;

  std::optional<GridInset> inset;
  std::vector<GridItemViewModel> children;
};

using GridChild = std::variant<GridItemViewModel, GridSectionModel>;

using GridSearchBarAccessory = std::variant<DropdownModel>;

struct GridModel {
  bool isLoading = false;
  bool filtering = false;
  bool throttle = false;
  double aspectRatio = 1.0;
  bool dirty = false;
  std::optional<int> columns;
  GridInset inset = GridInset::None;
  ObjectFit fit = ObjectFit::Contain;

  std::string navigationTitle;
  std::string searchPlaceholderText;
  std::optional<std::string> onSelectionChanged;
  std::optional<std::string> onSearchTextChange;
  std::optional<EventCounted<std::string>> searchText;
  std::vector<GridChild> items;
  std::optional<ActionPannelModel> actions;
  std::optional<EmptyViewModel> emptyView;
  std::optional<std::string> selectedItemId;
  std::optional<PaginationModel> pagination;
  std::optional<GridSearchBarAccessory> searchBarAccessory;
};
