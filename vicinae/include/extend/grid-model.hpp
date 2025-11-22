#pragma once
#include "extend/action-model.hpp"
#include "extend/empty-view-model.hpp"
#include "extend/image-model.hpp"
#include "extend/pagination-model.hpp"
#include "extend/dropdown-model.hpp"
#include "ui/image/image.hpp"
#include "ui/omni-grid/grid-item-content-widget.hpp"
#include <qjsonobject.h>

enum GridFit { GridContain, GridFill };

struct ImageContentWithTooltip {
  ImageLikeModel value;
  std::optional<QString> tooltip;
};

struct GridItemViewModel {
  using Content = std::variant<ImageLikeModel, ColorLike>;

  QString id;
  QString title;
  QString subtitle;
  Content content;
  std::optional<QString> tooltip;
  std::vector<QString> keywords;
  std::optional<ActionPannelModel> actionPannel;
};

struct GridSectionModel {
  QString title;
  QString subtitle;

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
  std::optional<GridSearchBarAccessory> searchBarAccessory;
};

class GridModelParser {
  GridItemContentWidget::Inset parseInset(const QString &s);
  GridItemViewModel parseListItem(const QJsonObject &instance, size_t index);
  GridSectionModel parseSection(const QJsonObject &instance);
  ObjectFit parseFit(const QString &fit);

public:
  GridModelParser();

  GridModel parse(const QJsonObject &instance);
};
