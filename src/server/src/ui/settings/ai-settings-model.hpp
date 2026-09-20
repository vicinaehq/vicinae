#pragma once
#include <algorithm>
#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <map>
#include <string>
#include <string_view>
#include <vector>
#include "services/ai/ai-preferences.hpp"

namespace AI {
class Service;
class AbstractProvider;
struct ProviderTypeInfo;
} // namespace AI
class RootItemManager;
class LocalStorageService;
class AISettingsModel;

struct AIProviderTypeRow {
  QString key;
  QString label;
  QString description;
  QVariant icon;
  bool builtin = false;
  bool allowMultiple = false;
  bool canAdd = false;
  QVariantList instances;
  bool operator==(const AIProviderTypeRow &) const = default;
};

struct AIProviderFieldRow {
  QString key;
  QString label;
  QString description;
  QString placeholder;
  QString kind;
  QVariantList options;
  QVariant value;
  QVariant currentOption;
  bool operator==(const AIProviderFieldRow &) const = default;
};

struct AIProviderModelRow {
  QString key;
  QString name;
  QString description;
  QString meta;
  QVariant icon;
  QString status;
  double progress = -1.0;
  bool operator==(const AIProviderModelRow &) const = default;
};

/**
 * Updates rows in place when only their content changes, so delegates (and their animations) survive
 * refreshes. Resets only when the set of rows changes.
 */
template <typename Row> class DiffListModel : public QAbstractListModel {
public:
  using QAbstractListModel::QAbstractListModel;

  int rowCount(const QModelIndex &parent = {}) const override {
    return parent.isValid() ? 0 : static_cast<int>(m_rows.size());
  }

  void setRows(std::vector<Row> rows) {
    const bool sameShape =
        rows.size() == m_rows.size() && std::ranges::equal(rows, m_rows, {}, &Row::key, &Row::key);
    if (!sameShape) {
      beginResetModel();
      m_rows = std::move(rows);
      endResetModel();
      return;
    }
    for (std::size_t i = 0; i < rows.size(); ++i) {
      if (rows[i] == m_rows[i]) continue;
      m_rows[i] = std::move(rows[i]);
      const auto idx = index(static_cast<int>(i));
      emit dataChanged(idx, idx);
    }
  }

protected:
  const Row *rowAt(const QModelIndex &index) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_rows.size())) return nullptr;
    return &m_rows[static_cast<std::size_t>(index.row())];
  }

  std::vector<Row> m_rows;
};

class AIProviderTypesModel : public DiffListModel<AIProviderTypeRow> {
  Q_OBJECT
  QML_NAMED_ELEMENT(AIProviderTypesModel)
  QML_UNCREATABLE("")

public:
  enum Roles {
    TypeRole = Qt::UserRole + 1,
    LabelRole,
    DescriptionRole,
    IconRole,
    BuiltinRole,
    AllowMultipleRole,
    CanAddRole,
    InstancesRole,
  };

  using DiffListModel::DiffListModel;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;
};

class AIProviderFieldsModel : public DiffListModel<AIProviderFieldRow> {
  Q_OBJECT
  QML_NAMED_ELEMENT(AIProviderFieldsModel)
  QML_UNCREATABLE("")

public:
  enum Roles {
    KeyRole = Qt::UserRole + 1,
    LabelRole,
    DescriptionRole,
    PlaceholderRole,
    KindRole,
    OptionsRole,
    ValueRole,
    CurrentOptionRole,
  };

  using DiffListModel::DiffListModel;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;
};

class AIProviderModelsModel : public DiffListModel<AIProviderModelRow> {
  Q_OBJECT
  QML_NAMED_ELEMENT(AIProviderModelsModel)
  QML_UNCREATABLE("")

public:
  enum Roles {
    IdRole = Qt::UserRole + 1,
    NameRole,
    DescriptionRole,
    MetaRole,
    IconRole,
    StatusRole,
    ProgressRole
  };

  using DiffListModel::DiffListModel;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;
};

/**
 * The provider currently open in the AI settings page.
 */
class AIProviderPage : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(AIProviderPage)
  QML_UNCREATABLE("")

  Q_PROPERTY(bool valid READ valid NOTIFY changed)
  Q_PROPERTY(QString providerId READ providerId NOTIFY changed)
  Q_PROPERTY(QString name READ name NOTIFY changed)
  Q_PROPERTY(QString typeLabel READ typeLabel NOTIFY changed)
  Q_PROPERTY(QString description READ description NOTIFY changed)
  Q_PROPERTY(QVariant icon READ icon NOTIFY changed)
  Q_PROPERTY(QString statusText READ statusText NOTIFY changed)
  Q_PROPERTY(bool builtin READ builtin NOTIFY changed)
  Q_PROPERTY(AIProviderFieldsModel *fields READ fields CONSTANT)
  Q_PROPERTY(AIProviderModelsModel *models READ models CONSTANT)

signals:
  void changed();

public:
  explicit AIProviderPage(AISettingsModel &owner);

  bool valid() const { return m_valid; }
  QString providerId() const { return m_providerId; }
  QString name() const { return m_name; }
  QString typeLabel() const { return m_typeLabel; }
  QString description() const { return m_description; }
  QVariant icon() const { return m_icon; }
  QString statusText() const { return m_statusText; }
  bool builtin() const { return m_builtin; }
  AIProviderFieldsModel *fields() { return &m_fields; }
  AIProviderModelsModel *models() { return &m_models; }

  Q_INVOKABLE void setField(const QString &key, const QVariant &value);
  Q_INVOKABLE void download(const QString &modelId);
  Q_INVOKABLE void cancelDownload(const QString &modelId);
  Q_INVOKABLE void removeModel(const QString &modelId);
  Q_INVOKABLE void remove();

private:
  friend class AISettingsModel;

  AISettingsModel &m_owner;
  bool m_valid = false;
  QString m_providerId;
  QString m_name;
  QString m_typeLabel;
  QString m_description;
  QVariant m_icon;
  QString m_statusText;
  bool m_builtin = false;
  AIProviderFieldsModel m_fields{this};
  AIProviderModelsModel m_models{this};
};

class AISettingsModel : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(AISettingsModel)
  QML_UNCREATABLE("")

  Q_PROPERTY(AIProviderTypesModel *configuredTypes READ configuredTypes CONSTANT)
  Q_PROPERTY(AIProviderTypesModel *availableTypes READ availableTypes CONSTANT)
  Q_PROPERTY(QString selectedProviderId READ selectedProviderId WRITE setSelectedProviderId NOTIFY
                 selectedProviderIdChanged)
  Q_PROPERTY(AIProviderPage *provider READ provider CONSTANT)
  Q_PROPERTY(AIProviderFieldsModel *setupFields READ setupFields CONSTANT)

signals:
  void selectedProviderIdChanged();

public:
  explicit AISettingsModel(QObject *parent = nullptr);

  AIProviderTypesModel *configuredTypes() { return &m_configuredTypes; }
  AIProviderTypesModel *availableTypes() { return &m_availableTypes; }
  QString selectedProviderId() const { return m_selectedProviderId; }
  void setSelectedProviderId(const QString &id);
  AIProviderPage *provider() { return &m_page; }
  AIProviderFieldsModel *setupFields() { return &m_setupFields; }

  Q_INVOKABLE QStringList prepareSetup(const QString &type);
  Q_INVOKABLE void addProvider(const QString &type, const QVariantMap &fields);
  Q_INVOKABLE QString nextProviderId(const QString &type) const;
  Q_INVOKABLE bool isProviderIdTaken(const QString &id) const;

private:
  friend class AIProviderPage;

  void removeProvider(const std::string &id);
  void setField(const std::string &id, const QString &key, const QVariant &value);

  std::map<std::string, AI::ProviderInstance> providers() const;
  void saveProviders(const std::map<std::string, AI::ProviderInstance> &providers);
  const AI::ProviderTypeInfo *typeInfoFor(const std::string &id) const;
  bool canAddType(const AI::ProviderTypeInfo &info) const;
  QString statusText(AI::AbstractProvider &provider) const;
  std::vector<AIProviderModelRow> modelRows(AI::AbstractProvider &provider) const;
  std::vector<AIProviderFieldRow> fieldRows(const std::string &id, const AI::ProviderTypeInfo &info,
                                            bool withValues) const;
  void rebuildTypes();
  void rebuildPage();
  void rebuildPageModels();

  AI::Service *m_aiService = nullptr;
  RootItemManager *m_rootItems = nullptr;
  LocalStorageService *m_storage = nullptr;
  QString m_selectedProviderId;
  AIProviderTypesModel m_configuredTypes{this};
  AIProviderTypesModel m_availableTypes{this};
  AIProviderPage m_page{*this};
  AIProviderFieldsModel m_setupFields{this};
};
