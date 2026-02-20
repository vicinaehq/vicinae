#pragma once
#include "common/entrypoint.hpp"
#include "qml-preference-form-model.hpp"
#include <QAbstractListModel>
#include <set>

class RootItemManager;

class QmlExtensionSettingsModel : public QAbstractListModel {
  Q_OBJECT

  Q_PROPERTY(QString selectedTitle READ selectedTitle NOTIFY selectedChanged)
  Q_PROPERTY(QString selectedIconSource READ selectedIconSource NOTIFY selectedChanged)
  Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY selectedChanged)
  Q_PROPERTY(bool hasPreferences READ hasPreferences NOTIFY selectedChanged)
  Q_PROPERTY(bool selectedIsProvider READ selectedIsProvider NOTIFY selectedChanged)
  Q_PROPERTY(bool selectedEnabled READ selectedEnabled NOTIFY selectedChanged)
  Q_PROPERTY(QString selectedAlias READ selectedAlias NOTIFY selectedChanged)
  Q_PROPERTY(QmlPreferenceFormModel *preferenceModel READ preferenceModel CONSTANT)
  Q_PROPERTY(int selectedRow READ selectedRow NOTIFY selectedChanged)

signals:
  void selectedChanged();

public:
  enum Role {
    NameRole = Qt::UserRole + 1,
    TypeRole,
    IconSourceRole,
    IsProviderRole,
    IndentRole,
    EnabledRole,
    AliasRole,
    EntrypointIdRole,
    ExpandedRole,
    ExpandableRole
  };

  explicit QmlExtensionSettingsModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  QString selectedTitle() const;
  QString selectedIconSource() const;
  bool hasSelection() const;
  bool hasPreferences() const;
  bool selectedIsProvider() const;
  bool selectedEnabled() const;
  QString selectedAlias() const;
  int selectedRow() const { return m_selectedRow; }
  QmlPreferenceFormModel *preferenceModel() const { return m_prefModel; }

  Q_INVOKABLE void setFilter(const QString &text);
  Q_INVOKABLE void select(int row);
  Q_INVOKABLE void setEnabled(int row, bool value);
  Q_INVOKABLE void setAlias(int row, const QString &alias);
  Q_INVOKABLE void selectByEntrypointId(const QString &id);
  Q_INVOKABLE void toggleExpanded(int row);

private:
  struct Entry {
    QString name;
    QString type;
    QString iconSource;
    bool isProvider;
    bool enabled;
    QString alias;
    EntrypointId entrypointId;
    QString providerId;
    int indent;
    bool expanded = true;
    int childCount = 0;
  };

  void rebuild(const QString &filter);
  void rebuildVisible();
  int childCountForProvider(int allIdx) const;

  std::vector<Entry> m_allEntries;
  std::vector<int> m_visibleIndices;
  int m_selectedRow = -1;
  QString m_filter;
  QmlPreferenceFormModel *m_prefModel;
  std::set<QString> m_expandedProviders;
};
