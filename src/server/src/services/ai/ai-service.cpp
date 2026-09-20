#include "ai-service.hpp"
#include <memory>
#include <qjsonobject.h>
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

std::string typeOf(const glz::generic::object_t &object) {
  return glazeToQJsonObject(object).value(QStringLiteral("type")).toString().toStdString();
}

bool isBuiltin(std::string_view type) {
  const auto *info = findProviderType(type);
  return info && info->cardinality == Cardinality::Builtin;
}

glz::generic defaultValue(const ProviderField &field) {
  if (field.kind == FieldKind::Toggle) return field.defaultChecked;
  return std::string(field.defaultValue);
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
  provider->configure(resolveFields(id, *provider));
  provider->start();
  m_providers[std::move(id)] = std::move(provider);
}

void Service::reconfigure(std::string_view id) {
  auto *provider = getProviderById(id);
  if (provider) provider->configure(resolveFields(id, *provider));
}

ProviderFields Service::resolveFields(std::string_view id, const AbstractProvider &provider) const {
  ProviderFields fields;
  const auto *info = findProviderType(provider.type());
  if (!info) return fields;

  const auto &providers = m_config.value().ai.providers;
  const auto entry = providers.find(std::string(id));
  const auto json = entry != providers.end() ? glazeToQJsonObject(entry->second) : QJsonObject{};

  for (const auto &field : info->fields) {
    const auto key = qs(field.key);
    const auto value =
        field.kind == FieldKind::Secret ? m_storage.getItem(secretScope(id), key) : json.value(key);
    fields.values[std::string(field.key)] =
        value.isUndefined() || value.isNull() ? defaultValue(field) : qJsonValueToGlazeGeneric(value);
  }
  return fields;
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

void Service::reconcile(const config::ConfigValue &current, const config::ConfigValue &previous) {
  const auto &next = current.ai.providers;
  const auto &prev = previous.ai.providers;

  std::erase_if(m_providers, [&](const auto &entry) {
    const auto &[id, provider] = entry;
    return !isBuiltin(provider->type()) && !next.contains(id);
  });

  for (const auto &[id, object] : next) {
    if (auto it = m_providers.find(id); it != m_providers.end()) {
      auto oldIt = prev.find(id);
      if (oldIt == prev.end() || glazeToQJsonObject(oldIt->second) != glazeToQJsonObject(object)) {
        it->second->configure(resolveFields(id, *it->second));
      }
      continue;
    }
    if (const auto type = typeOf(object); !isBuiltin(type)) instantiate(id, type);
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
