#include "ai-settings-model.hpp"
#include <algorithm>
#include <format>
#include <ranges>
#include "service-registry.hpp"
#include "services/ai/ai-config.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/ai/ai-service.hpp"

AISettingsModel::AISettingsModel(QObject *parent) : QAbstractListModel(parent) {
  m_aiService = ServiceRegistry::instance()->ai();
  if (m_aiService) {
    connect(&m_aiService->configManager(), &AI::ConfigManager::configChanged, this, &AISettingsModel::reload);
    reload();
  }
}

int AISettingsModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return static_cast<int>(m_entries.size());
}

QVariant AISettingsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_entries.size())) return {};

  const auto &entry = m_entries[static_cast<std::size_t>(index.row())];
  const auto &config = m_aiService->configManager().value();
  auto it = config.providers.find(entry.id);
  if (it == config.providers.end()) return {};

  auto *typeInfo = AI::findProviderType(entry.type);

  switch (role) {
  case ProviderIdRole:
    return QString::fromStdString(entry.id);
  case TypeRole:
    return QString::fromStdString(entry.type);
  case TypeLabelRole:
    if (typeInfo) return QString::fromUtf8(typeInfo->label.data(), typeInfo->label.size());
    return QString::fromStdString(entry.type);
  case IconRole:
    if (typeInfo) return QString::fromUtf8(typeInfo->icon.data(), typeInfo->icon.size());
    return QStringLiteral("computer-chip");
  case DescriptionRole: {
    auto *provider = m_aiService->getProviderById(entry.id);
    if (provider) {
      auto desc = provider->description();
      return QString::fromUtf8(desc.data(), desc.size());
    }
    if (typeInfo) return QString::fromUtf8(typeInfo->description.data(), typeInfo->description.size());
    return {};
  }
  case ExpandedRole:
    return entry.expanded;
  case UrlRole:
    if (auto *cfg = std::get_if<AI::ConfigValue::OllamaConfig>(&it->second)) {
      return QString::fromStdString(cfg->url);
    }
    return {};
  case ApiKeyRole:
    if (auto *cfg = std::get_if<AI::ConfigValue::MistralConfig>(&it->second)) {
      return QString::fromStdString(cfg->apiKey);
    }
    return {};
  default:
    return {};
  }
}

QHash<int, QByteArray> AISettingsModel::roleNames() const {
  return {
      {ProviderIdRole, "providerId"},
      {TypeRole, "type"},
      {TypeLabelRole, "typeLabel"},
      {IconRole, "icon"},
      {DescriptionRole, "description"},
      {ExpandedRole, "expanded"},
      {UrlRole, "url"},
      {ApiKeyRole, "apiKey"},
  };
}

bool AISettingsModel::hasProviders() const { return !m_entries.empty(); }

QVariantList AISettingsModel::availableTypes() const {
  QVariantList result;
  for (const auto &info : AI::kProviderTypes) {
    if (!canAddType(info.type)) continue;
    QVariantMap entry;
    entry[QStringLiteral("type")] = QString::fromUtf8(info.type.data(), info.type.size());
    entry[QStringLiteral("label")] = QString::fromUtf8(info.label.data(), info.label.size());
    entry[QStringLiteral("icon")] = QString::fromUtf8(info.icon.data(), info.icon.size());
    result.append(entry);
  }
  return result;
}

bool AISettingsModel::canAddType(std::string_view type) const {
  auto *typeInfo = AI::findProviderType(type);
  if (!typeInfo) return false;
  if (typeInfo->allowMultiple) return true;
  return std::ranges::none_of(m_entries, [type](const auto &e) { return e.type == type; });
}

void AISettingsModel::reload() {
  beginResetModel();
  m_entries.clear();
  if (m_aiService) {
    const auto &config = m_aiService->configManager().value();
    m_entries.reserve(config.providers.size());
    for (const auto &[id, variant] : config.providers) {
      auto type = std::visit([](const auto &cfg) { return cfg.type; }, variant);
      m_entries.push_back({.id = id, .type = type});
    }
  }
  endResetModel();
  emit providersChanged();
}

QString AISettingsModel::nextProviderId(const QString &type) const {
  auto typeStd = type.toStdString();
  const auto &config = m_aiService->configManager().value();

  int suffix = 1;
  std::string id;
  do {
    id = suffix == 1 ? typeStd : std::format("{}-{}", typeStd, suffix);
    ++suffix;
  } while (config.providers.contains(id));

  return QString::fromStdString(id);
}

void AISettingsModel::addProvider(const QString &type, const QVariantMap &fields) {
  auto typeStd = type.toStdString();
  if (!canAddType(typeStd)) return;

  auto *typeInfo = AI::findProviderType(typeStd);
  if (!typeInfo) return;

  auto &config = m_aiService->configManager().value();

  std::string id;
  if (fields.contains(QStringLiteral("id"))) { id = fields[QStringLiteral("id")].toString().toStdString(); }
  if (id.empty() || config.providers.contains(id)) { id = nextProviderId(type).toStdString(); }

  auto providerConfig = typeInfo->makeDefault();

  if (typeStd == "ollama") {
    if (auto *cfg = std::get_if<AI::ConfigValue::OllamaConfig>(&providerConfig)) {
      if (fields.contains(QStringLiteral("url")))
        cfg->url = fields[QStringLiteral("url")].toString().toStdString();
    }
  } else if (typeStd == "mistral") {
    if (auto *cfg = std::get_if<AI::ConfigValue::MistralConfig>(&providerConfig)) {
      if (fields.contains(QStringLiteral("apiKey")))
        cfg->apiKey = fields[QStringLiteral("apiKey")].toString().toStdString();
    }
  }

  config.providers[id] = std::move(providerConfig);
  save();
}

void AISettingsModel::removeProvider(int row) {
  if (row < 0 || row >= static_cast<int>(m_entries.size())) return;

  auto &config = m_aiService->configManager().value();
  config.providers.erase(m_entries[static_cast<std::size_t>(row)].id);
  save();
}

void AISettingsModel::toggleExpanded(int row) {
  if (row < 0 || row >= static_cast<int>(m_entries.size())) return;

  // Collapse all others
  for (std::size_t i = 0; i < m_entries.size(); ++i) {
    if (static_cast<int>(i) != row && m_entries[i].expanded) {
      m_entries[i].expanded = false;
      auto idx = index(static_cast<int>(i));
      emit dataChanged(idx, idx, {ExpandedRole});
    }
  }

  m_entries[static_cast<std::size_t>(row)].expanded = !m_entries[static_cast<std::size_t>(row)].expanded;
  auto idx = index(row);
  emit dataChanged(idx, idx, {ExpandedRole});
}

void AISettingsModel::setField(int row, const QString &field, const QString &value) {
  if (row < 0 || row >= static_cast<int>(m_entries.size())) return;

  auto &config = m_aiService->configManager().value();
  auto it = config.providers.find(m_entries[static_cast<std::size_t>(row)].id);
  if (it == config.providers.end()) return;

  auto fieldStd = field.toStdString();
  auto valueStd = value.toStdString();

  if (fieldStd == "url") {
    if (auto *cfg = std::get_if<AI::ConfigValue::OllamaConfig>(&it->second)) { cfg->url = valueStd; }
  } else if (fieldStd == "apiKey") {
    if (auto *cfg = std::get_if<AI::ConfigValue::MistralConfig>(&it->second)) { cfg->apiKey = valueStd; }
  } else {
    return;
  }

  save();
}

QVariantList AISettingsModel::modelsForProvider(int row) const {
  if (row < 0 || row >= static_cast<int>(m_entries.size())) return {};

  auto *provider = m_aiService->getProviderById(m_entries[static_cast<std::size_t>(row)].id);
  if (!provider) return {};

  QVariantList result;
  for (const auto &model : provider->listModels()) {
    QVariantMap m;
    m[QStringLiteral("id")] = QString::fromStdString(model.id);
    m[QStringLiteral("name")] = QString::fromStdString(model.name);
    auto caps = AI::stringifyCapabilities(model.caps);
    QStringList capList;
    capList.reserve(static_cast<int>(caps.size()));
    for (const auto &c : caps) {
      capList.append(QString::fromStdString(c));
    }
    m[QStringLiteral("capabilities")] = capList.join(QStringLiteral(", "));
    result.append(m);
  }
  return result;
}

void AISettingsModel::save() { m_aiService->configManager().save(); }
