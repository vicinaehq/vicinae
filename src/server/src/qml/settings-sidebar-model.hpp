#pragma once
#include <string>
#include <vector>
#include <QAbstractListModel>

class ExtensionSettingsModel;

// Flat list model backing the settings sidebar. Holds core pages, installed
// extensions and (while searching) the matching commands nested under their
// provider, plus divider rows. Filtering happens in C++ via setQuery().
class SettingsSidebarModel : public QAbstractListModel {
  Q_OBJECT

public:
  enum Role {
    KeyRole = Qt::UserRole + 1,
    KindRole,
    LabelRole,
    IconRole,
    IconSourceRole,
    EnabledRole,
  };

  explicit SettingsSidebarModel(ExtensionSettingsModel *extModel, QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE void setQuery(const QString &query);
  // Navigation helpers used by the sidebar's keyboard handling. They operate on
  // row indices so QML never has to materialize the row list.
  Q_INVOKABLE int indexOfKey(const QString &key) const;
  Q_INVOKABLE QString keyAt(int row) const;
  Q_INVOKABLE QString kindAt(int row) const;
  Q_INVOKABLE int firstSelectableRow() const;
  Q_INVOKABLE int stepRow(int fromRow, int delta) const;

private:
  struct Row {
    QString key;
    QString kind;
    QString label;
    QString icon;
    QString iconSource;
    bool enabled = true;
  };

  void rebuildRows();
  void setProviderEnabled(const QString &providerId, bool enabled);
  int lastSelectableRow() const;

  std::vector<Row> m_rows;
  std::string m_query;
};
