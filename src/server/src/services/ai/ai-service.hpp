#pragma once
#include <algorithm>
#include <memory>
#include <qobject.h>
#include <qtmetamacros.h>
#include "ai-provider.hpp"
#include "common/types.hpp"
#include "services/ai/ai-config.hpp"
#include "services/ai/ollama/ollama-ai-provider.hpp"
#include "services/ai/mistral/mistral-provider.hpp"
#include "vicinae.hpp"

namespace AI {
class Service : public QObject {
  Q_OBJECT

signals:
  void modelsChanged() const;

public:
  Service() : m_configManager(Omnicast::configDir() / "ai.json") {
    connect(&m_configManager, &ConfigManager::configChanged, this, &Service::reconcileProviders);
    m_configManager.load();
  }

  ~Service() override = default;

  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(const ChatCompletionPayload &payload) const {
    if (!payload.modelId.empty()) {
      if (payload.providerId) {
        if (auto it =
                std::ranges::find_if(m_providers, [&](auto &&p) { return p.first == *payload.providerId; });
            it != m_providers.end()) {
          return it->second->createChatCompletion(payload);
        }
      }

      for (const auto &[id, provider] : m_providers) {
        for (const auto &model : provider->listModels()) {
          if (model.id == payload.modelId) { return provider->createChatCompletion(payload); }
        }
      }
    }

    for (const auto &[id, provider] : m_providers) {
      if (const auto model = provider->findBestModel(AI::Capability::Completion)) {
        if (!isModelEnabled(id, model->id)) continue;
        return provider->createChatCompletion({.modelId = model->id, .messages = payload.messages});
      }
    }

    return nullptr;
  }

  QFuture<TranscriptionResult> transcribe(const std::filesystem::path &path) {
    for (const auto &[id, provider] : m_providers) {
      if (const auto model = provider->findBestModel(Capability::Transcription)) {
        if (!isModelEnabled(id, model->id)) continue;
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
        pmodel.enabled = isModelEnabled(id, pmodel.id);
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
      pmodel.enabled = isModelEnabled(pmodel.providerId, pmodel.id);
      models.emplace_back(std::move(pmodel));
    }
    return models;
  }

  bool isModelEnabled(const std::string &providerId, const std::string &modelId) const {
    auto modelKey = providerId + ":" + modelId;
    auto it = m_configManager.value().models.find(modelKey);
    return it == m_configManager.value().models.end() || it->second.enabled;
  }

private:
  void reconcileProviders(const ConfigValue &current, const ConfigValue &previous) {
    auto const &newProviders = current.providers;
    auto const &oldProviders = previous.providers;

    // Remove providers that no longer exist or whose config changed
    std::erase_if(m_providers, [&](const auto &entry) {
      auto const &[id, provider] = entry;
      auto it = newProviders.find(id);
      if (it == newProviders.end()) return true;
      auto oldIt = oldProviders.find(id);
      return oldIt == oldProviders.end() || oldIt->second != it->second;
    });

    // Add new providers or recreate changed ones
    std::vector<std::shared_ptr<AbstractProvider>> toStart;
    for (auto const &[id, config] : newProviders) {
      if (m_providers.contains(id)) continue;

      auto provider = createProvider(id, config);
      if (!provider) continue;
      connect(provider.get(), &AI::AbstractProvider::modelsUpdated, this, &Service::modelsChanged);
      toStart.emplace_back(provider);
      m_providers[id] = std::move(provider);
    }

    for (auto &provider : toStart) {
      provider->start();
    }
  }

  static std::shared_ptr<AI::AbstractProvider> createProvider(const std::string &id,
                                                              const ConfigValue::ProviderConfig &config) {
    return std::visit(
        overloads{[](AI::ConfigValue::OllamaConfig ollama) -> std::shared_ptr<AI::AbstractProvider> {
                    return std::make_shared<OllamaProvider>(std::move(ollama));
                  },
                  [](const AI::ConfigValue::MistralConfig &mistral) -> std::shared_ptr<AI::AbstractProvider> {
                    return std::make_shared<MistralProvider>(mistral.apiKey.c_str());
                  },
                  [](const auto &) -> std::shared_ptr<AI::AbstractProvider> { return nullptr; }},
        config);
  }

  std::unordered_map<std::string, std::shared_ptr<AI::AbstractProvider>> m_providers;
  ConfigManager m_configManager;
};
}; // namespace AI
