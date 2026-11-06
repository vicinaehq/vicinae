#pragma once
#include <format>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <qfuture.h>
#include <qimage.h>
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include "ai-preferences.hpp"
#include "ai-provider.hpp"
#include "common/types.hpp"
#include "services/audio/audio-recorder.hpp"

class LocalStorageService;

namespace AI {

class Service : public QObject, NonCopyable {
  Q_OBJECT

signals:
  void modelsChanged() const;
  void managedModelsChanged() const;

public:
  explicit Service(LocalStorageService &storage);
  ~Service() override = default;

  static QString secretScope(std::string_view providerId);

  void addProvider(std::unique_ptr<AbstractProvider> provider);
  void setProviders(std::map<std::string, ProviderInstance> providers);

  // secrets live outside the config file, so their edits are pushed explicitly
  void reconfigure(std::string_view id);

  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(std::optional<ModelRef> ref, const ChatCompletionPayload &payload) const {
    auto completion = createChatCompletionImpl(std::move(ref), payload);
    if (completion) { qInfo() << "created chat completion for model" << completion->model().id; }
    return completion;
  }

  void preloadModel(const ModelRef &ref) {
    if (auto it = m_providers.find(ref.provider); it != m_providers.end()) {
      it->second->preloadModel(ref.id);
    }
  }

  void cancelPreload(const ModelRef &ref) {
    if (auto it = m_providers.find(ref.provider); it != m_providers.end()) {
      it->second->cancelPreload(ref.id);
    }
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

  std::vector<AI::ProviderModel> listModels(std::optional<Capabilities> caps = std::nullopt) {
    std::vector<AI::ProviderModel> models;
    models.reserve(m_providers.size() * 50);

    for (const auto &[id, provider] : m_providers) {
      for (auto &model : provider->listModels()) {
        if (caps && !(model.caps & *caps)) continue;
        models.emplace_back(id, std::move(model));
      }
    }

    return models;
  }

  bool isAvailable(const ModelRef &ref, Capabilities caps) {
    const auto models = listModels(caps);
    return std::ranges::any_of(models, [&](const ProviderModel &model) {
      return model.ref.provider == ref.provider && model.ref.id == ref.id;
    });
  }

private:
  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletionImpl(std::optional<ModelRef> ref, const ChatCompletionPayload &payload) const {
    if (ref) {
      if (auto it = m_providers.find(ref->provider); it != m_providers.end()) {
        return it->second->createChatCompletion(ref->id, payload);
      }
    }
    return nullptr;
  }

  static std::unique_ptr<AbstractProvider> createProvider(const std::string &id, std::string_view type);
  PreferenceValues resolveFields(std::string_view id, const AbstractProvider &provider) const;
  void instantiate(const std::string &id, std::string_view type);

  LocalStorageService &m_storage;
  std::map<std::string, ProviderInstance> m_instances;
  std::unordered_map<std::string, std::unique_ptr<AI::AbstractProvider>> m_providers;
};
}; // namespace AI
