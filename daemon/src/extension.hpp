#pragma once
#include <QList>
#include <QString>
#include <QWidget>
#include <cmath>
#include <qjsonobject.h>
#include <qtmetamacros.h>

struct ThemeIconModel {
  QString iconName;
  QString theme;
};

struct ImageFileModel {
  QString path;
};

struct ImageUrlModel {
  QString url;
};

using ImageLikeModel =
    std::variant<ImageUrlModel, ThemeIconModel, ImageFileModel>;

struct ColorStringModel {
  QString colorString;
};

struct ThemeColorModel {
  QString themeColor;
};

using ColorLikeModel = QString;

struct TagItemModel {
  QString text;
  std::optional<ImageLikeModel> icon;
  std::optional<ColorLikeModel> color;
  QString onAction;
};

struct TagListModel {
  QString title;
  QList<TagItemModel> items;
};

struct MetadataLabel {
  QString text;
  QString title;
};

struct MetadataSeparator {};

using MetadataItem =
    std::variant<MetadataLabel, MetadataSeparator, TagListModel>;

struct ActionModel {
  QString title;
  QString onAction;
  std::optional<ImageLikeModel> icon;
};

struct ActionPannelSectionModel {
  QList<ActionModel> actions;
};

struct ActionPannelSubmenuModel {
  QString title;
  std::optional<ImageLikeModel> icon;
  QString onOpen;
  QString onSearchTextChange;
  QList<std::variant<ActionPannelSectionModel, ActionModel>> children;
};

using ActionPannelItem = std::variant<ActionModel, ActionPannelSectionModel,
                                      ActionPannelSubmenuModel>;

struct ActionPannelModel {
  QString title;
  QList<ActionPannelItem> children;
};

struct ListItemDetail {
  QString markdown;
  QList<MetadataItem> metadata;
};

struct ListItemViewModel {
  bool changed;
  QString id;
  QString title;
  QString subtitle;
  ImageLikeModel icon;
  std::optional<ListItemDetail> detail;
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

class ExtensionComponent : public QWidget {
  Q_OBJECT

public:
signals:
  void extensionEvent(const QString &action, const QJsonObject &payload);
};
