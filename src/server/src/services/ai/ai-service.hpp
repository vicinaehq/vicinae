#pragma once
#include <memory>
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include "ai-provider-registry.hpp"
#include "ai-provider.hpp"
#include "common/types.hpp"
#include "services/ai/ai-config.hpp"
#include "vicinae.hpp"

namespace AI {
class Service : public QObject, NonCopyable {
  Q_OBJECT

signals:
  void modelsChanged() const;

public:
  Service() : m_registry(makeBuiltinRegistry()), m_configManager(Omnicast::configDir() / "ai.json") {
    connect(&m_configManager, &ConfigManager::configChanged, this, &Service::reconcileProviders);
    m_configManager.load();
  }

  ~Service() override = default;

  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(std::optional<ModelRef> ref, const ChatCompletionPayload &payload) const {
    auto completion = createChatCompletionImpl(std::move(ref), payload);
    if (completion) { qInfo() << "created chat completion for model" << completion->model().id; }
    return completion;
  }

  QFuture<std::expected<TranscriptionResponse, std::string>> transcribe(const std::filesystem::path &path) {
    for (const auto &[id, provider] : m_providers) {
      if (const auto model = provider->findBestModel(Capability::Transcription)) {
        if (!isModelEnabled(ModelRef{id, model->id})) continue;
        return provider->transcribe(path);
      }
    }

    return {};
  }

  AbstractProvider *getProviderById(std::string_view id) {
    if (auto it = m_providers.find(std::string(id)); it != m_providers.end()) { return it->second.get(); }
    return nullptr;
  }

  ConfigManager &configManager() { return m_configManager; }

  const auto &providers() const { return m_providers; }

  std::vector<AI::ProviderModel> listModels(std::optional<Capabilities> caps = std::nullopt) {
    std::vector<AI::ProviderModel> models;
    models.reserve(m_providers.size() * 50);

    for (const auto &[id, provider] : m_providers) {
      for (auto &model : provider->listModels()) {
        if (caps && !(model.caps & *caps)) continue;
        ProviderModel pmodel(id, std::move(model));
        pmodel.enabled = isModelEnabled(pmodel.ref);
        if (!pmodel.enabled) continue;
        models.emplace_back(std::move(pmodel));
      }
    }

    return models;
  }

  std::vector<AI::ProviderModel> modelsForProvider(std::string_view providerId) {
    auto it = m_providers.find(std::string(providerId));
    if (it == m_providers.end()) return {};

    std::vector<AI::ProviderModel> models;
    for (auto &model : it->second->listModels()) {
      ProviderModel pmodel(std::string(providerId), std::move(model));
      pmodel.enabled = isModelEnabled(pmodel.ref);
      models.emplace_back(std::move(pmodel));
    }
    return models;
  }

  bool isModelEnabled(const ModelRef &ref) const {
    auto it = m_configManager.value().models.find(ref.toString());
    return it == m_configManager.value().models.end() || it->second.enabled;
  }

private:
  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletionImpl(std::optional<ModelRef> ref, const ChatCompletionPayload &payload) const {
    if (ref) {
      if (auto it = m_providers.find(ref->provider); it != m_providers.end()) {
        return it->second->createChatCompletion(ref->id, payload);
      }
      return nullptr;
    }

    for (const auto &[id, provider] : m_providers) {
      if (const auto model = provider->findBestModel(AI::Capability::Completion)) {
        if (!isModelEnabled(ModelRef{id, model->id})) continue;
        return provider->createChatCompletion(model->id, payload);
      }
    }

    return nullptr;
  }

  void reconcileProviders(const ConfigValue &current, const ConfigValue &previous) {
    auto const &newProviders = current.providers;
    auto const &oldProviders = previous.providers;

    std::erase_if(m_providers, [&](const auto &entry) {
      auto const &[id, provider] = entry;
      auto it = newProviders.find(id);
      if (it == newProviders.end()) return true;
      auto oldIt = oldProviders.find(id);
      return oldIt == oldProviders.end() || oldIt->second != it->second;
    });

    std::vector<AbstractProvider *> toStart;
    for (auto const &[id, config] : newProviders) {
      if (m_providers.contains(id)) continue;

      auto provider = m_registry.create(config);
      if (!provider) continue;
      connect(provider.get(), &AI::AbstractProvider::modelsUpdated, this, &Service::modelsChanged);
      toStart.emplace_back(provider.get());
      m_providers[id] = std::move(provider);
    }

    for (auto *provider : toStart) {
      provider->start();
    }
  }

  ProviderRegistry m_registry;
  std::unordered_map<std::string, std::unique_ptr<AI::AbstractProvider>> m_providers;
  ConfigManager m_configManager;
};
}; // namespace AI
