#include "ai-service.hpp"
#include <memory>
#include <utility>
#include <glaze/json/write.hpp>
#include <qstring.h>
#include "internal/glaze-qt.hpp"
#include "services/ai/ai-provider-types.hpp"
#include "services/ai/mistral/mistral-provider.hpp"
#include "services/ai/ollama/ollama-ai-provider.hpp"
#include "services/ai/groq/groq-provider.hpp"
#include "services/ai/openai/openai-compatible-provider.hpp"
#include "services/ai/openai/openai-provider.hpp"
#include "services/ai/openrouter/openrouter-provider.hpp"
#include "services/local-storage/local-storage-service.hpp"

namespace AI {

namespace {
QString qs(std::string_view view) {
  return QString::fromUtf8(view.data(), static_cast<qsizetype>(view.size()));
}

bool isBuiltin(std::string_view type) {
  const auto *info = findProviderType(type);
  return info && info->cardinality == Cardinality::Builtin;
}

std::string serialize(const ProviderInstance &instance) {
  std::string json;
  if (glz::write_json(instance, json)) return {};
  return json;
}
} // namespace

Service::Service(LocalStorageService &storage) : m_storage(storage) {}

QString Service::secretScope(std::string_view providerId) {
  return QStringLiteral("ai:%1").arg(qs(providerId));
}

void Service::addProvider(std::unique_ptr<AbstractProvider> provider) {
  auto id = provider->id();
  connect(provider.get(), &AI::AbstractProvider::modelsUpdated, this, &Service::modelsChanged);
  connect(provider.get(), &AI::AbstractProvider::managedModelsChanged, this, &Service::managedModelsChanged);
  provider->configure(resolveFields(id, *provider));
  provider->start();
  m_providers[std::move(id)] = std::move(provider);
}

void Service::reconfigure(std::string_view id) {
  auto *provider = getProviderById(id);
  if (provider) provider->configure(resolveFields(id, *provider));
}

PreferenceValues Service::resolveFields(std::string_view id, const AbstractProvider &provider) const {
  const auto *info = findProviderType(provider.type());
  if (!info) return {};

  PreferenceValues values;
  if (auto it = m_instances.find(std::string(id)); it != m_instances.end()) values = it->second.fields;

  for (const ::Preference &pref : info->fields()) {
    const auto key = pref.name().toStdString();
    if (pref.isSecret()) {
      auto secret = qJsonValueToGlazeGeneric(m_storage.getItem(secretScope(id), pref.name()));
      if (!secret.is_null()) values[key] = std::move(secret);
    } else if (!values.contains(key)) {
      values[key] = pref.defaultOrNull();
    }
  }
  return values;
}

void Service::instantiate(const std::string &id, std::string_view type) {
  auto provider = createProvider(id, type);
  if (!provider) {
    qWarning() << "Unknown AI provider type" << type << "for provider" << id;
    return;
  }
  connect(provider.get(), &AI::AbstractProvider::modelsUpdated, this, &Service::modelsChanged);
  connect(provider.get(), &AI::AbstractProvider::managedModelsChanged, this, &Service::managedModelsChanged);
  auto *raw = provider.get();
  m_providers[id] = std::move(provider);
  raw->configure(resolveFields(id, *raw));
  raw->start();
}

void Service::setProviders(std::map<std::string, ProviderInstance> providers) {
  const auto previous = std::exchange(m_instances, std::move(providers));

  std::erase_if(m_providers, [&](const auto &entry) {
    const auto &[id, provider] = entry;
    return !isBuiltin(provider->type()) && !m_instances.contains(id);
  });

  for (const auto &[id, instance] : m_instances) {
    if (auto it = m_providers.find(id); it != m_providers.end()) {
      auto oldIt = previous.find(id);
      if (oldIt == previous.end() || serialize(oldIt->second) != serialize(instance)) {
        it->second->configure(resolveFields(id, *it->second));
      }
      continue;
    }
    if (!isBuiltin(instance.type)) instantiate(id, instance.type);
  }

  emit modelsChanged();
}

std::unique_ptr<AbstractProvider> Service::createProvider(const std::string &id, std::string_view type) {
  if (type == "ollama") return std::make_unique<OllamaProvider>(id);
  if (type == "mistral") return std::make_unique<MistralProvider>(id);
  if (type == "openai") return std::make_unique<OpenAIProvider>(id);
  if (type == "groq") return std::make_unique<GroqProvider>(id);
  if (type == "openrouter") return std::make_unique<OpenRouterProvider>(id);
  if (type == "openai-compatible") return std::make_unique<OpenAICompatibleProvider>(id, type, "");
  return nullptr;
}

} // namespace AI
