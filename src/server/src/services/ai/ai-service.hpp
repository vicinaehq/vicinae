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
    connect(&m_configManager, &ConfigManager::configChanged, this,
            [this]() { applyConfig(m_configManager.value()); });
    m_configManager.load();
  }

  ~Service() override = default;

  // for now, we reinstantiate all providers on config change
  // this happens rarely enough so that this isn't a problem.
  // this greately simplifies thing: we can directly pass the config to the provider's constructor and we do
  // not have to handle reconfiguration. All ongoing generation tasks will be aborted.
  void applyConfig(const ConfigValue &value) {

    for (auto &provider : m_providers) {
      disconnect(provider.get());
    }

    m_providers.clear();

    for (auto const &[id, config] : value.providers) {
      auto provider = createProvider(config);
      connect(provider.get(), &AI::AbstractProvider::modelsUpdated, this, &Service::modelsChanged);
      m_providers.emplace_back(provider);
    }

    for (auto &provider : m_providers) {
      provider->start();
    }
  }

  std::shared_ptr<AI::AbstractProvider> createProvider(const ConfigValue::ProviderConfig &config) {
    auto const visitor =
        overloads{[](AI::ConfigValue::OllamaConfig ollama) -> std::shared_ptr<AI::AbstractProvider> {
                    return std::make_shared<OllamaProvider>(std::move(ollama));
                  },
                  [](const AI::ConfigValue::MistralConfig &mistral) -> std::shared_ptr<AI::AbstractProvider> {
                    return std::make_shared<MistralProvider>(mistral.apiKey.c_str());
                  },
                  [](const auto &p) { return nullptr; }};

    return std::visit(visitor, config);
  }

  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(const ChatCompletionPayload &payload) const {
    if (!payload.modelId.empty()) {
      if (payload.providerId) {
        if (auto it =
                std::ranges::find_if(m_providers, [&](auto &&p) { return p->id() == *payload.providerId; });
            it != m_providers.end()) {
          return (*it)->createChatCompletion(payload);
        }
      }

      for (const auto &provider : m_providers) {
        for (const auto &model : provider->listModels()) {
          if (model.id == payload.modelId) { return provider->createChatCompletion(payload); }
        }
      }
    }

    for (const auto &provider : m_providers) {
      if (const auto model = provider->findBestModel(AI::Capability::Completion)) {
        return provider->createChatCompletion({.modelId = model->id, .messages = payload.messages});
      }
    }

    return nullptr;
  }

  QFuture<TranscriptionResult> transcribe(const std::filesystem::path &path) {
    for (const auto &provider : m_providers) {
      if (const auto model = provider->findBestModel(Capability::Transcription)) {
        return provider->transcribe(path);
      }
    }

    return {};
  }

  AbstractProvider *getProviderById(std::string_view id) {
    if (auto it = std::ranges::find_if(m_providers, [&](auto &&provider) { return provider->id() == id; });
        it != m_providers.end()) {
      return it->get();
    }

    return nullptr;
  }

  const auto &providers() const { return m_providers; }

  std::vector<AI::ProviderModel> listModels(std::optional<Capabilities> caps = std::nullopt) {
    std::vector<AI::ProviderModel> models;

    models.reserve(m_providers.size() * 50);

    for (const auto &provider : m_providers) {
      for (auto &model : provider->listModels()) {
        if (caps && !(model.caps & *caps)) continue;
        ProviderModel pmodel(provider->id(), std::move(model));
        models.emplace_back(pmodel);
      }
    }

    return models;
  }

private:
  std::vector<std::shared_ptr<AI::AbstractProvider>> m_providers;
  ConfigManager m_configManager;
};
}; // namespace AI
