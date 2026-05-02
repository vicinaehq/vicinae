#pragma once
#include "section-source.hpp"
#include "view-scope.hpp"
#include "theme.hpp"
#include <QAbstractListModel>
#include <memory>
#include <vector>

class SectionListModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(bool selectFirstOnReset READ selectFirstOnReset NOTIFY selectFirstOnResetChanged)
  Q_PROPERTY(bool awaitingData READ awaitingData NOTIFY awaitingDataChanged)
  Q_PROPERTY(int selectedIndex READ selectedIndex NOTIFY selectedIndexChanged)

signals:
  void selectFirstOnResetChanged();
  void awaitingDataChanged();
  void selectedIndexChanged();

public:
  enum Role {
    IsSection = Qt::UserRole + 1,
    IsSelectable,
    SectionName,
    Title,
    Subtitle,
    IconSource,
    Accessory,
  };

  explicit SectionListModel(QObject *parent = nullptr);

  void setScope(const ViewScope &scope) { m_scope = scope; }
  void addSource(SectionSource *source);
  void clearSources();

  void rebuild();
  void setFilter(const QString &text);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  bool selectFirstOnReset() const { return m_selectFirstOnReset; }
  void setSelectFirstOnReset(bool value);
  bool awaitingData() const { return m_awaitingData; }
  int selectedIndex() const { return m_selectedIndex; }

  Q_INVOKABLE void setSelectedIndex(int index);
  Q_INVOKABLE void activateSelected();
  Q_INVOKABLE int nextSelectableIndex(int from, int direction) const;
  Q_INVOKABLE int nextSectionIndex(int from, int direction) const;
  Q_INVOKABLE int scrollTargetIndex(int index, int direction) const;

  void refreshActionPanel();
  void beforePop();

  virtual QUrl qmlComponentUrl() const { return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml")); }

protected:
  bool dataItemAt(int row, int &sourceIdx, int &itemIdx) const;
  const std::vector<SectionSource *> &sources() const { return m_sources; }

private:
  struct FlatItem {
    enum Kind { SectionHeader, DataItem } kind;
    int sourceIdx;
    int itemIdx;
  };

  void rebuildFlatList();

  ViewScope m_scope;
  std::vector<SectionSource *> m_sources;
  std::vector<FlatItem> m_flat;
  int m_selectedIndex = -1;
  QString m_lastSelectedItemId;
  bool m_selectFirstOnReset = true;
  bool m_awaitingData = true;
};
