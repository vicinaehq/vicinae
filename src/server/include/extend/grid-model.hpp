#pragma once
#include "extend/action-model.hpp"
#include "extend/empty-view-model.hpp"
#include "extend/event-counted.hpp"
#include "extend/image-model.hpp"
#include "extend/node-tree.hpp"
#include "extend/pagination-model.hpp"
#include "extend/dropdown-model.hpp"
#include "ui/image/url.hpp"

enum GridFit { GridContain, GridFill };

enum class GridInset { None, Small, Medium, Large };

struct ImageContentWithTooltip {
  ImageLikeModel value;
  std::optional<std::string> tooltip;
};

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
  bool dirty = true;
  bool isLoading;
  bool filtering;
  bool throttle;
  double aspectRatio;
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

class GridModelParser {
  GridInset parseInset(const std::string &s);
  GridItemViewModel parseGridItem(const Node &node, const NodeTree &tree, size_t index);
  GridSectionModel parseSection(const Node &node, const NodeTree &tree);
  ObjectFit parseFit(const std::string &fit);

public:
  GridModel parse(const Node &node, const NodeTree &tree);
};
