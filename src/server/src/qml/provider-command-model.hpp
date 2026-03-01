#pragma once
#include <QAbstractListModel>
#include <QString>
#include <vector>

class ProviderCommandModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

signals:
  void countChanged();

public:
  enum Role {
    NameRole = Qt::UserRole + 1,
    TypeRole,
    IconSourceRole,
    EnabledRole,
    AliasRole,
    EntrypointIdRole,
    DescriptionRole
  };

  struct Command {
    QString name;
    QString type;
    QString iconSource;
    QString description;
    bool enabled;
    QString alias;
    QString entrypointId;
  };

  explicit ProviderCommandModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  void load(std::vector<Command> commands);
  void clear();

  bool setEnabled(const QString &entrypointId, bool value);
  bool setAlias(const QString &entrypointId, const QString &alias);

private:
  int findByEntrypointId(const QString &id) const;
  std::vector<Command> m_commands;
};
