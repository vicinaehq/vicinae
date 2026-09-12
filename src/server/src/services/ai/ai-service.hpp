#pragma once
#include <format>
#include <memory>
#include <unordered_set>
#include <qfuture.h>
#include <qimage.h>
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include "ai-provider-registry.hpp"
#include "ai-provider.hpp"
#include "common/types.hpp"
#include "services/ai/ai-config.hpp"
#include "services/ai/local-speech/local-speech-provider.hpp"
#include "services/audio/audio-recorder.hpp"
#include "vicinae.hpp"

namespace AI {
class Service : public QObject, NonCopyable {
  Q_OBJECT

signals:
  void modelsChanged() const;

public:
  explicit Service(LocalSpeechModelRegistry &speechModels)
      : m_registry(makeBuiltinRegistry()), m_configManager(Omnicast::configDir() / "ai.json") {
    addBuiltinProvider(std::make_unique<LocalSpeechProvider>(speechModels));
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

  QFuture<AI::Result<TranscriptionResponse>> transcribe(Audio::Recording recording, const QString &mime) {

    for (const auto &[id, provider] : m_providers) {
      if (const auto model = provider->findBestModel(Capability::Transcription)) {
        if (!isModelEnabled(ModelRef{id, model->id})) continue;
        return provider->transcribe(std::move(recording));
      }
    }

    return {};
  }

  QFuture<AI::Result<TranscriptionResponse>> transcribe(const ModelRef &ref, Audio::Recording recording,
                                                        TranscriptionOptions opts = {}) {
    auto *provider = getProviderById(ref.provider);
    if (!provider) {
      return QtFuture::makeReadyValueFuture<AI::Result<TranscriptionResponse>>(
          std::unexpected(std::format("Unknown AI provider '{}'", ref.provider)));
    }
    opts.model = ref.id;
    return provider->transcribe(std::move(recording), opts);
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

  void addBuiltinProvider(std::unique_ptr<AbstractProvider> provider) {
    auto id = provider->id();
    connect(provider.get(), &AI::AbstractProvider::modelsUpdated, this, &Service::modelsChanged);
    provider->start();
    m_builtinProviders.insert(id);
    m_providers[std::move(id)] = std::move(provider);
  }

  void reconcileProviders(const ConfigValue &current, const ConfigValue &previous) {
    auto const &newProviders = current.providers;
    auto const &oldProviders = previous.providers;

    std::erase_if(m_providers, [&](const auto &entry) {
      auto const &[id, provider] = entry;
      if (m_builtinProviders.contains(id)) return false;
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
  std::unordered_set<std::string> m_builtinProviders;
  ConfigManager m_configManager;
};
}; // namespace AI
