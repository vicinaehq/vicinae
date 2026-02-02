#pragma once
#include "extend/action-model.hpp"
#include "extend/empty-view-model.hpp"
#include "extend/image-model.hpp"
#include "extend/pagination-model.hpp"
#include "extend/dropdown-model.hpp"
#include "ui/image/url.hpp"
#include "ui/omni-grid/grid-item-content-widget.hpp"
#include <qjsonobject.h>

enum GridFit { GridContain, GridFill };

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

  std::optional<GridItemContentWidget::Inset> inset;
  std::vector<GridItemViewModel> children;
};

using GridChild = std::variant<GridItemViewModel, GridSectionModel>;

using GridSearchBarAccessory = std::variant<DropdownModel>;

struct GridModel {
  bool isLoading;
  bool filtering;
  bool throttle;
  double aspectRatio;
  bool dirty;
  std::optional<int> columns;
  GridItemContentWidget::Inset inset = GridItemContentWidget::Inset::None;
  ObjectFit fit = ObjectFit::Contain;

  std::string navigationTitle;
  std::string searchPlaceholderText;
  std::optional<std::string> onSelectionChanged;
  std::optional<std::string> onSearchTextChange;
  std::optional<std::string> searchText;
  std::vector<GridChild> items;
  std::optional<ActionPannelModel> actions;
  std::optional<EmptyViewModel> emptyView;
  std::optional<std::string> selectedItemId;
  std::optional<PaginationModel> pagination;
  std::optional<GridSearchBarAccessory> searchBarAccessory;
};

class GridModelParser {
  GridItemContentWidget::Inset parseInset(const std::string &s);
  GridItemViewModel parseListItem(const QJsonObject &instance, size_t index);
  GridSectionModel parseSection(const QJsonObject &instance);
  ObjectFit parseFit(const std::string &fit);

public:
  GridModelParser();

  GridModel parse(const QJsonObject &instance);
};
