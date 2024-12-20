#pragma once
#include <QList>
#include <QString>
#include <QWidget>
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

struct MetadataLabel {
  QString text;
  QString title;
};

struct MetadataSeparator {};

using MetadataItem = std::variant<MetadataLabel, MetadataSeparator>;

struct ActionModel {
  QString title;
  QString onAction;
  std::optional<ImageLikeModel> icon;
};

struct ActionPannelDivider {};

using ActionPannelItem = std::variant<ActionModel, ActionPannelDivider>;

struct ActionPannel {
  QString title;
  QList<ActionModel> actions;
};

struct ListItemDetail {
  QString markdown;
  QList<MetadataItem> metadata;
};

struct ListItemViewModel {
  QString id;
  QString title;
  QString subtitle;
  std::optional<ImageLikeModel> icon;
  std::optional<ListItemDetail> detail;
  std::optional<ActionPannel> actionPannel;
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
