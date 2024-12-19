#pragma once
#include <QList>
#include <QString>
#include <QWidget>
#include <qjsonobject.h>
#include <qtmetamacros.h>

struct MetadataLabel {
  QString text;
  QString title;
};

struct MetadataSeparator {};

using MetadataItem = std::variant<MetadataLabel, MetadataSeparator>;

struct ListItemDetail {
  QString markdown;
  QList<MetadataItem> metadata;
};

struct ListItemViewModel {
  QString id;
  QString title;
  QString subtitle;
  std::optional<ListItemDetail> detail;
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
