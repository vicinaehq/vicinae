#pragma once
#include <QAbstractListModel>
#include <QString>
#include <vector>

class ProviderCommandModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
  Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)

signals:
  void countChanged();
  void totalCountChanged();

public:
  enum Role {
    NameRole = Qt::UserRole + 1,
    TypeRole,
    IconSourceRole,
    EnabledRole,
    AliasRole,
    EntrypointIdRole,
    DescriptionRole,
    HasPreferencesRole
  };

  struct Command {
    QString name;
    QString type;
    QString iconSource;
    QString description;
    bool enabled;
    bool hasPreferences;
    QString alias;
    QString entrypointId;
  };

  explicit ProviderCommandModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = {}) const override;
  int totalCount() const { return static_cast<int>(m_allCommands.size()); }
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  void load(std::vector<Command> commands);
  void clear();

  bool setEnabled(const QString &entrypointId, bool value);
  bool setAlias(const QString &entrypointId, const QString &alias);

  Q_INVOKABLE int findByEntrypointId(const QString &id) const;
  Q_INVOKABLE void setFilter(const QString &text);

private:
  void rebuildVisible();
  int visibleRowFor(int allIdx) const;

  std::vector<Command> m_allCommands;
  std::vector<int> m_visibleIndices;
  QString m_filter;
};
