#pragma once
#include "grid-source.hpp"
#include "view-scope.hpp"
#include <QAbstractListModel>
#include <cstdint>
#include <memory>
#include <vector>

class ActionPanelState;

class SectionGridModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(int selectedSection READ selectedSection NOTIFY selectionChanged)
  Q_PROPERTY(int selectedItem READ selectedItem NOTIFY selectionChanged)
  Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
  Q_PROPERTY(double aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
  Q_PROPERTY(bool awaitingData READ awaitingData NOTIFY awaitingDataChanged)

public:
  enum Role : std::uint16_t {
    IsSection = Qt::UserRole + 1,
    SectionNameRole,
    RowSectionIdx,
    RowStartItem,
    RowItemCount,
    RowColumnsRole,
    RowAspectRatioRole,
  };

  explicit SectionGridModel(QObject *parent = nullptr);

  void setScope(const ViewScope &scope) { m_scope = scope; }
  void addSource(GridSource *source);
  void clearSources();

  void rebuild();
  void setFilter(const QString &text);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  int selectedSection() const { return m_selSection; }
  int selectedItem() const { return m_selItem; }
  bool awaitingData() const { return m_awaitingData; }
  int columns() const { return m_columns; }
  double aspectRatio() const { return m_aspectRatio; }

  void setColumns(int cols);
  void setAspectRatio(double ratio);
  void setSelectFirstOnReset(bool value) { m_selectFirstOnReset = value; }

  Q_INVOKABLE void select(int section, int item);
  Q_INVOKABLE void activateSelected();
  Q_INVOKABLE void navigateUp();
  Q_INVOKABLE void navigateDown();
  Q_INVOKABLE void navigateLeft();
  Q_INVOKABLE void navigateRight();
  Q_INVOKABLE void navigateSectionUp();
  Q_INVOKABLE void navigateSectionDown();
  Q_INVOKABLE int flatRowForSelection() const;
  Q_INVOKABLE bool alignSelectionScrollToTop() const;

  void refreshActionPanel();
  void selectFirst();
  void beforePop() {}

signals:
  void selectionChanged();
  void columnsChanged();
  void aspectRatioChanged();
  void awaitingDataChanged();

protected:
  virtual void onSelectionCleared();
  const ViewScope &scope() const { return m_scope; }

  bool resolveSelection(int section, int item, int &sourceIdx, int &itemIdx) const;
  const std::vector<GridSource *> &sources() const { return m_sources; }

private:
  struct SectionInfo {
    int sourceIdx;
    int count;
    QString name;
    std::optional<int> columns;
    std::optional<double> aspectRatio;
  };

  struct FlatRow {
    enum Kind : uint8_t { SectionHeader, ItemRow };
    Kind kind;
    int sectionIdx;
    QString sectionName;
    int startItem = 0;
    int itemCount = 0;
    int columns = 0;
    double aspectRatio = 1.0;
  };

  void rebuildFromSources();
  std::vector<FlatRow> buildFlatList() const;
  void rebuildRows();
  int sectionColumns(int sectionIdx) const;
  int nextNonEmptySection(int sectionIdx, int direction) const;
  void selectSectionBoundary(int sectionIdx, bool endOfSection, bool revealHeader = true);
  int totalItemCount() const;
  int toGlobal(int section, int item) const;
  void fromGlobal(int globalIdx, int &section, int &item) const;

  ViewScope m_scope;
  std::vector<GridSource *> m_sources;
  std::vector<SectionInfo> m_sections;
  std::vector<FlatRow> m_rows;
  int m_selSection = -1;
  int m_selItem = -1;
  int m_columns = 8;
  double m_aspectRatio = 1.0;
  bool m_selectFirstOnReset = false;
  bool m_awaitingData = true;
  bool m_preferSectionHeaderForSelection = false;
  bool m_alignSelectionScrollToTop = false;
};
