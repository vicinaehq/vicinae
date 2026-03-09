#pragma once
#include <QAbstractListModel>
#include <QString>
#include <QVariantList>
#include <vector>

namespace AI {
class Service;
}

class AISettingsModel : public QAbstractListModel {
  Q_OBJECT

  Q_PROPERTY(bool hasProviders READ hasProviders NOTIFY providersChanged)
  Q_PROPERTY(QVariantList availableTypes READ availableTypes NOTIFY providersChanged)

public:
  enum Roles {
    ProviderIdRole = Qt::UserRole + 1,
    TypeRole,
    TypeLabelRole,
    IconRole,
    DescriptionRole,
    UrlRole,
    ApiKeyRole,
  };

  explicit AISettingsModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  bool hasProviders() const;
  QVariantList availableTypes() const;

  Q_INVOKABLE void addProvider(const QString &type, const QVariantMap &fields);
  Q_INVOKABLE void removeProvider(int row);
  Q_INVOKABLE void setField(int row, const QString &field, const QString &value);
  Q_INVOKABLE QVariantMap providerDetails(int row) const;
  Q_INVOKABLE QVariantList modelsForProvider(int row) const;
  Q_INVOKABLE void setModelEnabled(int row, const QString &modelId, bool enabled);
  Q_INVOKABLE QString nextProviderId(const QString &type) const;
  Q_INVOKABLE bool isProviderIdTaken(const QString &id) const;

signals:
  void providersChanged();
  void modelsConfigChanged();

private:
  struct ProviderEntry {
    std::string id;
    std::string type;
  };

  bool canAddType(std::string_view type) const;
  void reload();
  void save();

  AI::Service *m_aiService = nullptr;
  std::vector<ProviderEntry> m_entries;
};
