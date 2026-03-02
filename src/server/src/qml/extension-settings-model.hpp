#pragma once
#include "common/entrypoint.hpp"
#include "preference-form-model.hpp"
#include "provider-command-model.hpp"
#include <QAbstractListModel>
#include <set>

class RootProvider;

class RootItemManager;

class ExtensionSettingsModel : public QAbstractListModel {
  Q_OBJECT

  Q_PROPERTY(QString selectedTitle READ selectedTitle NOTIFY selectedChanged)
  Q_PROPERTY(QString selectedIconSource READ selectedIconSource NOTIFY selectedChanged)
  Q_PROPERTY(QString selectedDescription READ selectedDescription NOTIFY selectedChanged)
  Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY selectedChanged)
  Q_PROPERTY(bool hasPreferences READ hasPreferences NOTIFY selectedChanged)
  Q_PROPERTY(bool selectedIsProvider READ selectedIsProvider NOTIFY selectedChanged)
  Q_PROPERTY(bool selectedEnabled READ selectedEnabled NOTIFY selectedChanged)
  Q_PROPERTY(QString selectedAlias READ selectedAlias NOTIFY selectedChanged)
  Q_PROPERTY(PreferenceFormModel *preferenceModel READ preferenceModel CONSTANT)
  Q_PROPERTY(PreferenceFormModel *commandPreferenceModel READ commandPreferenceModel CONSTANT)
  Q_PROPERTY(int selectedRow READ selectedRow NOTIFY selectedChanged)
  Q_PROPERTY(QString selectedProviderId READ selectedProviderId NOTIFY selectedChanged)
  Q_PROPERTY(QString selectedProvenance READ selectedProvenance NOTIFY selectedChanged)
  Q_PROPERTY(ProviderCommandModel *commandModel READ commandModel CONSTANT)

signals:
  void selectedChanged();
  void providerEnabledChanged(const QString &providerId, bool enabled);

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
    ExpandableRole,
    DescriptionRole,
    ProvenanceRole
  };

  explicit ExtensionSettingsModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  QString selectedTitle() const;
  QString selectedIconSource() const;
  QString selectedDescription() const;
  bool hasSelection() const;
  bool hasPreferences() const;
  bool selectedIsProvider() const;
  bool selectedEnabled() const;
  QString selectedAlias() const;
  int selectedRow() const { return m_selectedRow; }
  PreferenceFormModel *preferenceModel() const { return m_prefModel; }
  PreferenceFormModel *commandPreferenceModel() const { return m_cmdPrefModel; }
  QString selectedProviderId() const;
  QString selectedProvenance() const;
  ProviderCommandModel *commandModel() const { return m_commandModel; }

  Q_INVOKABLE void setFilter(const QString &text);
  Q_INVOKABLE void select(int row);
  Q_INVOKABLE void setEnabled(int row, bool value);
  Q_INVOKABLE void setAlias(int row, const QString &alias);
  Q_INVOKABLE void toggleExpanded(int row);
  Q_INVOKABLE void moveUp();
  Q_INVOKABLE void moveDown();
  Q_INVOKABLE void activate();
  Q_INVOKABLE void selectProviderById(const QString &providerId);
  Q_INVOKABLE void setEnabledByEntrypointId(const QString &id, bool value);
  Q_INVOKABLE void setAliasByEntrypointId(const QString &id, const QString &alias);
  Q_INVOKABLE void loadCommandPreferences(const QString &entrypointId);

private:
  struct Entry {
    QString name;
    QString type;
    QString iconSource;
    QString description;
    bool isProvider;
    bool enabled;
    QString alias;
    EntrypointId entrypointId;
    QString providerId;
    int indent;
    QString provenance;
    bool expanded = true;
    int childCount = 0;
  };

  void rebuild(const QString &filter);
  void rebuildVisible();
  void loadCommandsForProvider(const QString &providerId);
  int findVisibleEntryByEntrypointId(const QString &id) const;

public:
  static QString provenanceForProvider(RootProvider *provider);

private:

  std::vector<Entry> m_allEntries;
  std::vector<int> m_visibleIndices;
  int m_selectedRow = -1;
  QString m_filter;
  PreferenceFormModel *m_prefModel;
  PreferenceFormModel *m_cmdPrefModel;
  ProviderCommandModel *m_commandModel;
  std::set<QString> m_expandedProviders;
};
