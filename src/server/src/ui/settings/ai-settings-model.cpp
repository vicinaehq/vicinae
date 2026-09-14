#include "ai-settings-model.hpp"
#include <algorithm>
#include <format>
#include <ranges>
#include <qcoreapplication.h>
#include <qjsonobject.h>
#include <qlogging.h>
#include "config/config.hpp"
#include "internal/glaze-qt.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-provider-types.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/ai/ai-service.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "ui/image/image-url.hpp"
#include "utils/utils.hpp"

namespace {

QString qs(std::string_view view) {
  return QString::fromUtf8(view.data(), static_cast<qsizetype>(view.size()));
}

QStringList capabilityNames(AI::Capabilities caps) {
  QStringList names;
  for (const auto &name : AI::stringifyCapabilities(caps)) {
    auto text = QString::fromStdString(name);
    if (!text.isEmpty()) text[0] = text[0].toUpper();
    names << text;
  }
  return names;
}

QString joinMeta(const QStringList &parts) { return parts.join(QStringLiteral(" · ")); }

QJsonObject providerObject(const config::Manager &config, const std::string &id) {
  const auto &providers = config.value().ai.providers;
  auto it = providers.find(id);
  return it == providers.end() ? QJsonObject{} : glazeToQJsonObject(it->second);
}

std::string providerType(const glz::generic::object_t &object) {
  return glazeToQJsonObject(object).value(QStringLiteral("type")).toString().toStdString();
}

QVariant iconVariant(const std::optional<ImageUrl> &icon) {
  return icon ? QVariant::fromValue(*icon) : QVariant();
}

QVariantMap instanceEntry(const QString &id, const QString &name, const QString &status) {
  return {{QStringLiteral("id"), id}, {QStringLiteral("name"), name}, {QStringLiteral("statusText"), status}};
}

} // namespace

// ── list models ──

QVariant AIProviderTypesModel::data(const QModelIndex &index, int role) const {
  const auto *row = rowAt(index);
  if (!row) return {};
  switch (role) {
  case TypeRole:
    return row->key;
  case LabelRole:
    return row->label;
  case DescriptionRole:
    return row->description;
  case IconRole:
    return row->icon;
  case BuiltinRole:
    return row->builtin;
  case AllowMultipleRole:
    return row->allowMultiple;
  case CanAddRole:
    return row->canAdd;
  case InstancesRole:
    return row->instances;
  default:
    return {};
  }
}

QHash<int, QByteArray> AIProviderTypesModel::roleNames() const {
  return {
      {TypeRole, "type"},     {LabelRole, "label"},         {DescriptionRole, "description"},
      {IconRole, "icon"},     {BuiltinRole, "builtin"},     {AllowMultipleRole, "allowMultiple"},
      {CanAddRole, "canAdd"}, {InstancesRole, "instances"},
  };
}

QVariant AIProviderFieldsModel::data(const QModelIndex &index, int role) const {
  const auto *row = rowAt(index);
  if (!row) return {};
  switch (role) {
  case KeyRole:
    return row->key;
  case LabelRole:
    return row->label;
  case DescriptionRole:
    return row->description;
  case PlaceholderRole:
    return row->placeholder;
  case SecretRole:
    return row->secret;
  case ValueRole:
    return row->value;
  default:
    return {};
  }
}

QHash<int, QByteArray> AIProviderFieldsModel::roleNames() const {
  return {
      {KeyRole, "key"},
      {LabelRole, "label"},
      {DescriptionRole, "description"},
      {PlaceholderRole, "placeholder"},
      {SecretRole, "secret"},
      {ValueRole, "value"},
  };
}

QVariant AIProviderModelsModel::data(const QModelIndex &index, int role) const {
  const auto *row = rowAt(index);
  if (!row) return {};
  switch (role) {
  case IdRole:
    return row->key;
  case NameRole:
    return row->name;
  case DescriptionRole:
    return row->description;
  case MetaRole:
    return row->meta;
  case IconRole:
    return row->icon;
  case StatusRole:
    return row->status;
  case ProgressRole:
    return row->progress;
  default:
    return {};
  }
}

QHash<int, QByteArray> AIProviderModelsModel::roleNames() const {
  return {
      {IdRole, "modelId"}, {NameRole, "name"},     {DescriptionRole, "description"}, {MetaRole, "meta"},
      {IconRole, "icon"},  {StatusRole, "status"}, {ProgressRole, "progress"},
  };
}

// ── provider page ──

AIProviderPage::AIProviderPage(AISettingsModel &owner) : QObject(&owner), m_owner(owner) {}

void AIProviderPage::setField(const QString &key, const QString &value) {
  if (m_valid) m_owner.setField(m_providerId.toStdString(), key, value);
}

void AIProviderPage::download(const QString &modelId) {
  auto *provider =
      m_owner.m_aiService ? m_owner.m_aiService->getProviderById(m_providerId.toStdString()) : nullptr;
  if (!provider) return;
  if (auto result = provider->downloadModel(modelId.toStdString()); !result) {
    qWarning() << "Could not start model download:" << result.error();
  }
}

void AIProviderPage::cancelDownload(const QString &modelId) {
  auto *provider =
      m_owner.m_aiService ? m_owner.m_aiService->getProviderById(m_providerId.toStdString()) : nullptr;
  if (provider) provider->cancelDownload(modelId.toStdString());
}

void AIProviderPage::removeModel(const QString &modelId) {
  auto *provider =
      m_owner.m_aiService ? m_owner.m_aiService->getProviderById(m_providerId.toStdString()) : nullptr;
  if (!provider) return;
  if (auto result = provider->removeModel(modelId.toStdString()); !result) {
    qWarning() << "Could not remove model:" << result.error();
  }
}

void AIProviderPage::remove() {
  if (m_valid && !m_builtin) m_owner.removeProvider(m_providerId.toStdString());
}

// ── settings model ──

AISettingsModel::AISettingsModel(QObject *parent) : QObject(parent) {
  auto *registry = ServiceRegistry::instance();
  m_aiService = registry->ai();
  m_config = registry->config();
  m_storage = registry->localStorage();

  if (m_config) {
    connect(m_config, &config::Manager::configChanged, this, [this]() {
      rebuildTypes();
      rebuildPage();
    });
  }
  if (m_aiService) {
    connect(m_aiService, &AI::Service::modelsChanged, this, [this]() {
      rebuildTypes();
      rebuildPage();
    });
    connect(m_aiService, &AI::Service::managedModelsChanged, this, [this]() { rebuildPageModels(); });
  }

  rebuildTypes();
  rebuildPage();
}

void AISettingsModel::setSelectedProviderId(const QString &id) {
  if (m_selectedProviderId == id) return;
  m_selectedProviderId = id;
  rebuildPage();
  emit selectedProviderIdChanged();
}

bool AISettingsModel::canAddType(std::string_view type) const {
  auto *info = AI::findProviderType(type);
  if (!info || !m_config) return false;
  if (info->allowMultiple) return true;
  const auto &providers = m_config->value().ai.providers;
  return std::ranges::none_of(providers,
                              [type](const auto &entry) { return providerType(entry.second) == type; });
}

QString AISettingsModel::statusText(AI::AbstractProvider &provider) const {
  if (provider.managesModels()) {
    const auto models = provider.managedModels();
    const auto installed =
        std::ranges::count(models, AI::ManagedModel::State::Installed, &AI::ManagedModel::state);
    if (installed == 0) return tr("No models installed");
    return tr("%n model(s) installed", nullptr, static_cast<int>(installed));
  }
  const auto count = provider.listModels().size();
  if (count == 0) return tr("No models found");
  return tr("%n model(s)", nullptr, static_cast<int>(count));
}

std::vector<AIProviderModelRow> AISettingsModel::modelRows(AI::AbstractProvider &provider) const {
  std::vector<AIProviderModelRow> rows;

  if (provider.managesModels()) {
    const auto models = provider.managedModels();
    rows.reserve(models.size());
    for (const auto &model : models) {
      QStringList meta = capabilityNames(model.caps);
      if (model.size > 0) meta << formatSize(model.size);
      if (!model.precision.empty()) meta << QString::fromStdString(model.precision);
      if (!model.languages.empty()) meta << QString::fromStdString(model.languages);

      QString status;
      switch (model.state) {
      case AI::ManagedModel::State::Absent:
        status = QStringLiteral("absent");
        break;
      case AI::ManagedModel::State::Downloading:
        status = QStringLiteral("downloading");
        break;
      case AI::ManagedModel::State::Installed:
        status = QStringLiteral("installed");
        break;
      }

      rows.emplace_back(AIProviderModelRow{
          .key = QString::fromStdString(model.id),
          .name = QString::fromStdString(model.name),
          .description = QString::fromStdString(model.description),
          .meta = joinMeta(meta),
          .icon = iconVariant(model.icon),
          .status = status,
          .progress = model.progress,
      });
    }
    return rows;
  }

  const auto providerIcon = provider.icon();
  auto models = provider.listModels();
  rows.reserve(models.size());
  for (auto &model : models) {
    rows.emplace_back(AIProviderModelRow{
        .key = QString::fromStdString(model.id),
        .name = QString::fromStdString(model.name),
        .description = model.description ? QString::fromStdString(*model.description) : QString(),
        .meta = joinMeta(capabilityNames(model.caps)),
        .icon = iconVariant(model.icon ? model.icon : providerIcon),
        .status = QStringLiteral("available"),
    });
  }
  return rows;
}

std::vector<AIProviderFieldRow> AISettingsModel::fieldRows(const std::string &id, std::string_view type,
                                                           bool withValues) const {
  std::vector<AIProviderFieldRow> rows;
  auto *info = AI::findProviderType(type);
  if (!info) return rows;

  const auto object = withValues && m_config ? providerObject(*m_config, id) : QJsonObject{};
  const auto scope = AI::Service::secretScope(id);

  rows.reserve(info->fields.size());
  for (const auto &field : info->fields) {
    const auto key = qs(field.key);
    AIProviderFieldRow row{
        .key = key,
        .label = QCoreApplication::translate(AI::PROVIDER_TR_CONTEXT, field.label),
        .description = QCoreApplication::translate(AI::PROVIDER_TR_CONTEXT, field.description),
        .placeholder = qs(field.placeholder),
        .secret = field.secret,
    };
    if (withValues) {
      row.value = field.secret && m_storage ? m_storage->getItem(scope, key).toString()
                                            : object.value(key).toString();
    }
    rows.emplace_back(std::move(row));
  }
  return rows;
}

void AISettingsModel::rebuildTypes() {
  std::vector<AIProviderTypeRow> rows;

  if (m_aiService) {
    std::vector<AI::AbstractProvider *> statics;
    for (const auto &[id, provider] : m_aiService->providers()) {
      if (m_aiService->isStatic(id)) statics.push_back(provider.get());
    }
    std::ranges::sort(statics, {}, &AI::AbstractProvider::id);

    for (auto *provider : statics) {
      const auto id = QString::fromStdString(provider->id());
      const auto name = QString::fromStdString(provider->displayName());
      rows.emplace_back(AIProviderTypeRow{
          .key = id,
          .label = name,
          .description = qs(provider->description()),
          .icon = iconVariant(provider->icon()),
          .builtin = true,
          .instances = {instanceEntry(id, name, statusText(*provider))},
      });
    }
  }

  for (const auto &info : AI::PROVIDER_TYPES) {
    QVariantList instances;
    if (m_config) {
      for (const auto &[id, object] : m_config->value().ai.providers) {
        if (providerType(object) != info.type) continue;
        auto *provider = m_aiService ? m_aiService->getProviderById(id) : nullptr;
        const auto qid = QString::fromStdString(id);
        instances.append(instanceEntry(qid, qid, provider ? statusText(*provider) : tr("Not connected")));
      }
    }
    rows.emplace_back(AIProviderTypeRow{
        .key = qs(info.type),
        .label = qs(info.label),
        .description = qs(info.description),
        .icon = QVariant::fromValue(ImageUrl(ImageURL::builtin(info.icon))),
        .allowMultiple = info.allowMultiple,
        .canAdd = canAddType(info.type),
        .instances = std::move(instances),
    });
  }

  m_types.setRows(std::move(rows));
}

void AISettingsModel::rebuildPage() {
  auto &page = m_page;
  const auto id = m_selectedProviderId.toStdString();
  auto *provider = m_aiService && !id.empty() ? m_aiService->getProviderById(id) : nullptr;

  page.m_valid = false;
  page.m_providerId = m_selectedProviderId;
  page.m_builtin = provider && m_aiService->isStatic(id);

  if (page.m_builtin) {
    page.m_valid = true;
    page.m_name = QString::fromStdString(provider->displayName());
    page.m_typeLabel = page.m_name;
    page.m_description = qs(provider->description());
    page.m_icon = iconVariant(provider->icon());
    page.m_statusText = statusText(*provider);
    page.m_fields.setRows({});
  } else if (m_config && m_config->value().ai.providers.contains(id)) {
    const auto type = providerType(m_config->value().ai.providers.at(id));
    auto *info = AI::findProviderType(type);
    page.m_valid = true;
    page.m_name = m_selectedProviderId;
    page.m_typeLabel = info ? qs(info->label) : QString::fromStdString(type);
    page.m_description = provider ? qs(provider->description()) : (info ? qs(info->description) : QString());
    page.m_icon = info ? QVariant::fromValue(ImageUrl(ImageURL::builtin(info->icon))) : QVariant();
    page.m_statusText = provider ? statusText(*provider) : tr("Not connected");
    page.m_fields.setRows(fieldRows(id, type, true));
  } else {
    page.m_name.clear();
    page.m_typeLabel.clear();
    page.m_description.clear();
    page.m_icon = QVariant();
    page.m_statusText.clear();
    page.m_fields.setRows({});
  }

  rebuildPageModels();
  emit page.changed();
}

void AISettingsModel::rebuildPageModels() {
  const auto id = m_selectedProviderId.toStdString();
  auto *provider = m_aiService && !id.empty() ? m_aiService->getProviderById(id) : nullptr;
  m_page.m_models.setRows(provider ? modelRows(*provider) : std::vector<AIProviderModelRow>{});
}

QStringList AISettingsModel::prepareSetup(const QString &type) {
  auto rows = fieldRows({}, type.toStdString(), false);
  QStringList keys;
  for (const auto &row : rows) {
    keys << row.key;
  }
  m_setupFields.setRows(std::move(rows));
  return keys;
}

QString AISettingsModel::nextProviderId(const QString &type) const {
  auto typeStd = type.toStdString();
  const auto &providers = m_config->value().ai.providers;

  int suffix = 1;
  std::string id;
  do {
    id = suffix == 1 ? typeStd : std::format("{}-{}", typeStd, suffix);
    ++suffix;
  } while (providers.contains(id));

  return QString::fromStdString(id);
}

bool AISettingsModel::isProviderIdTaken(const QString &id) const {
  return m_config->value().ai.providers.contains(id.toStdString());
}

void AISettingsModel::addProvider(const QString &type, const QVariantMap &fields) {
  auto typeStd = type.toStdString();
  if (!canAddType(typeStd)) return;

  auto *typeInfo = AI::findProviderType(typeStd);
  if (!typeInfo) return;

  std::string id;
  if (fields.contains(QStringLiteral("id"))) { id = fields[QStringLiteral("id")].toString().toStdString(); }
  if (id.empty() || isProviderIdTaken(QString::fromStdString(id))) {
    id = nextProviderId(type).toStdString();
  }

  QJsonObject object;
  object[QStringLiteral("type")] = type;
  const auto scope = AI::Service::secretScope(id);

  for (const auto &field : typeInfo->fields) {
    const auto key = qs(field.key);
    if (!fields.contains(key)) continue;
    const auto value = fields[key].toString();
    if (field.secret) {
      m_storage->setItem(scope, key, value);
    } else {
      object[key] = value;
    }
  }

  m_config->updateUser([&](config::Partial<config::ConfigValue> &user) {
    if (!user.ai) user.ai.emplace();
    if (!user.ai->providers) user.ai->providers.emplace();
    (*user.ai->providers)[id] = qJsonObjectToGlazeGeneric(object);
  });
}

void AISettingsModel::removeProvider(const std::string &id) {
  m_storage->clearNamespace(AI::Service::secretScope(id));
  m_config->updateUser([&](config::Partial<config::ConfigValue> &user) {
    if (user.ai && user.ai->providers) user.ai->providers->erase(id);
  });
}

void AISettingsModel::setField(const std::string &id, const QString &key, const QString &value) {
  const auto &providers = m_config->value().ai.providers;
  auto entry = providers.find(id);
  if (entry == providers.end()) return;

  auto *typeInfo = AI::findProviderType(providerType(entry->second));
  if (!typeInfo) return;

  const auto keyStd = key.toStdString();
  auto it = std::ranges::find(typeInfo->fields, keyStd, &AI::ProviderField::key);
  if (it == typeInfo->fields.end()) return;

  if (it->secret) {
    m_storage->setItem(AI::Service::secretScope(id), key, value);
    m_aiService->reloadProvider(id);
    return;
  }

  auto object = providerObject(*m_config, id);
  object[key] = value;
  m_config->updateUser([&](config::Partial<config::ConfigValue> &user) {
    if (!user.ai) user.ai.emplace();
    if (!user.ai->providers) user.ai->providers.emplace();
    (*user.ai->providers)[id] = qJsonObjectToGlazeGeneric(object);
  });
}
