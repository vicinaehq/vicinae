#include "ai-service.hpp"
#include <memory>
#include <qjsonobject.h>
#include <qstring.h>
#include "internal/glaze-qt.hpp"
#include "services/ai/ai-provider-types.hpp"
#include "services/ai/mistral/mistral-provider.hpp"
#include "services/ai/ollama/ollama-ai-provider.hpp"
#include "services/local-storage/local-storage-service.hpp"

namespace AI {

namespace {
QString qs(std::string_view view) {
  return QString::fromUtf8(view.data(), static_cast<qsizetype>(view.size()));
}
} // namespace

Service::Service(config::Manager &config, LocalStorageService &storage)
    : m_config(config), m_storage(storage) {
  connect(&m_config, &config::Manager::configChanged, this, &Service::reconcile);
  reconcile(m_config.value(), {});
}

QString Service::secretScope(std::string_view providerId) {
  return QStringLiteral("ai:%1").arg(qs(providerId));
}

void Service::addProvider(std::unique_ptr<AbstractProvider> provider) {
  auto id = provider->id();
  connect(provider.get(), &AI::AbstractProvider::modelsUpdated, this, &Service::modelsChanged);
  connect(provider.get(), &AI::AbstractProvider::managedModelsChanged, this, &Service::managedModelsChanged);
  provider->start();
  m_staticProviders.insert(id);
  m_providers[std::move(id)] = std::move(provider);
}

void Service::reloadProvider(std::string_view id) {
  const auto key = std::string(id);
  if (m_staticProviders.contains(key)) return;
  m_providers.erase(key);
  const auto &providers = m_config.value().ai.providers;
  if (auto it = providers.find(key); it != providers.end()) instantiate(key, it->second);
  emit modelsChanged();
}

ProviderFields Service::resolveFields(std::string_view id, const glz::generic::object_t &object) const {
  const auto json = glazeToQJsonObject(object);
  const auto type = json.value(QStringLiteral("type")).toString().toStdString();
  const auto *info = findProviderType(type);
  if (!info) return {};

  ProviderFields fields;
  for (const auto &field : info->fields) {
    const auto key = qs(field.key);
    const auto value = field.secret ? m_storage.getItem(secretScope(id), key) : json.value(key);
    fields[std::string(field.key)] = value.toString().toStdString();
  }
  return fields;
}

void Service::instantiate(const std::string &id, const glz::generic::object_t &object) {
  const auto json = glazeToQJsonObject(object);
  const auto type = json.value(QStringLiteral("type")).toString().toStdString();
  auto provider = createProvider(type, resolveFields(id, object));
  if (!provider) {
    qWarning() << "Unknown AI provider type" << type << "for provider" << id;
    return;
  }
  connect(provider.get(), &AI::AbstractProvider::modelsUpdated, this, &Service::modelsChanged);
  connect(provider.get(), &AI::AbstractProvider::managedModelsChanged, this, &Service::managedModelsChanged);
  auto *raw = provider.get();
  m_providers[id] = std::move(provider);
  raw->start();
}

void Service::reconcile(const config::ConfigValue &current, const config::ConfigValue &previous) {
  const auto &next = current.ai.providers;
  const auto &prev = previous.ai.providers;

  std::erase_if(m_providers, [&](const auto &entry) {
    const auto &[id, provider] = entry;
    if (m_staticProviders.contains(id)) return false;
    auto it = next.find(id);
    if (it == next.end()) return true;
    auto oldIt = prev.find(id);
    return oldIt == prev.end() || glazeToQJsonObject(oldIt->second) != glazeToQJsonObject(it->second);
  });

  for (const auto &[id, object] : next) {
    if (!m_providers.contains(id)) instantiate(id, object);
  }

  emit modelsChanged();
}

std::unique_ptr<AbstractProvider> Service::createProvider(std::string_view type,
                                                          const ProviderFields &fields) {
  auto get = [&](std::string_view key) -> std::string {
    auto it = fields.find(std::string(key));
    return it == fields.end() ? std::string() : it->second;
  };
  if (type == "ollama") return std::make_unique<OllamaProvider>(get("url"));
  if (type == "mistral") return std::make_unique<MistralProvider>(QString::fromStdString(get("apiKey")));
  return nullptr;
}

} // namespace AI
