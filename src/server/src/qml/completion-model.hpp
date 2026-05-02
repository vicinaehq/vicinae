#pragma once
#include <QAbstractListModel>
#include <QVariantList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>
#include <string>
#include <vector>

class CompletionModel : public QAbstractListModel {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
  enum Role {
    ItemType = Qt::UserRole + 1,
    Title,
    IconSource,
    ItemData,
  };

  explicit CompletionModel(QObject *parent = nullptr);

  Q_INVOKABLE void setItems(const QVariantList &items);
  Q_INVOKABLE void setSections(const QVariantList &sections);
  Q_INVOKABLE void setFilter(const QString &query);
  Q_INVOKABLE int nextSelectableIndex(int from, int direction) const;
  Q_INVOKABLE QVariantMap itemDataAt(int index) const;

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  struct Item {
    std::string title;
    QString iconSource;
    QVariantMap data;
  };

signals:
  void countChanged();

private:
  struct Section {
    QString name;
    std::vector<Item> items;
  };

  struct FlatItem {
    enum Kind { SectionHeader, Entry } kind;
    int sectionIdx = -1;
    int itemIdx = -1;
  };

  void rebuildFlatList();

  std::string m_filterQuery;
  std::vector<Section> m_sections;
  std::vector<FlatItem> m_flat;
};
