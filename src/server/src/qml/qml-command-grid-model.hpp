#pragma once
#include "common/context.hpp"
#include "qml-utils.hpp"
#include <QAbstractListModel>
#include <QUrl>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

class ActionPanelState;

class QmlCommandGridModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(int selectedSection READ selectedSection NOTIFY selectionChanged)
  Q_PROPERTY(int selectedItem READ selectedItem NOTIFY selectionChanged)
  Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
  Q_PROPERTY(double aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)

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

  explicit QmlCommandGridModel(QObject *parent = nullptr);

  // --- QAbstractListModel ---
  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  // --- QmlBridgeView interface ---
  virtual void initialize(ApplicationContext *ctx);
  virtual void setFilter(const QString &text) = 0;
  virtual QString searchPlaceholder() const { return QStringLiteral("Search..."); }
  virtual QUrl qmlComponentUrl() const = 0;
  virtual void beforePop() {}
  void refreshActionPanel();

  // --- Selection & navigation ---
  int selectedSection() const { return m_selSection; }
  int selectedItem() const { return m_selItem; }
  Q_INVOKABLE void select(int section, int item);
  Q_INVOKABLE void activateSelected();
  Q_INVOKABLE void navigateUp();
  Q_INVOKABLE void navigateDown();
  Q_INVOKABLE void navigateLeft();
  Q_INVOKABLE void navigateRight();
  Q_INVOKABLE int flatRowForSelection() const;

  int columns() const { return m_columns; }
  void setColumns(int cols);

  double aspectRatio() const { return m_aspectRatio; }
  void setAspectRatio(double ratio);

signals:
  void selectionChanged();
  void columnsChanged();
  void aspectRatioChanged();

protected:
  struct SectionInfo {
    QString name;
    int count;
    std::optional<int> columns;
    std::optional<double> aspectRatio;
  };

  void setSections(const std::vector<SectionInfo> &sections);
  void selectFirst();

  virtual std::unique_ptr<ActionPanelState> createActionPanel(int section, int item) const = 0;
  virtual void onItemSelected(int section, int item) {}

  ApplicationContext *ctx() const { return m_ctx; }
  const std::vector<SectionInfo> &sections() const { return m_sections; }
  QString imageSourceFor(const ImageURL &url) const { return qml::imageSourceFor(url); }

private:
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

  void rebuildRows();
  int sectionColumns(int sectionIdx) const;
  int totalItemCount() const;
  int toGlobal(int section, int item) const;
  void fromGlobal(int globalIdx, int &section, int &item) const;

  ApplicationContext *m_ctx = nullptr;
  std::vector<SectionInfo> m_sections;
  std::vector<FlatRow> m_rows;
  int m_selSection = -1;
  int m_selItem = -1;
  int m_columns = 8;
  double m_aspectRatio = 1.0;
};
