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
#include "ai-provider.hpp"
#include "common/types.hpp"
#include "config/config.hpp"
#include "services/audio/audio-recorder.hpp"

class LocalStorageService;

namespace AI {

class Service : public QObject, NonCopyable {
  Q_OBJECT

signals:
  void modelsChanged() const;
  void managedModelsChanged() const;

public:
  Service(config::Manager &config, LocalStorageService &storage);
  ~Service() override = default;

  static QString secretScope(std::string_view providerId);

  void addProvider(std::unique_ptr<AbstractProvider> provider);

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

  QFuture<std::string> runCompletion(std::optional<ModelRef> ref,
                                     const ChatCompletionPayload &payload) const {
    auto promise = std::make_shared<QPromise<std::string>>();
    auto future = promise->future();
    m_currentCompletion = createChatCompletionImpl(std::move(ref), payload);

    if (!m_currentCompletion) {
      qWarning() << "runCompletion: failed to create completion";
      return QtFuture::makeReadyValueFuture<std::string>("Could not start completion");
    }

    auto str = new std::string{};

    m_currentCompletion->start();

    connect(m_currentCompletion.get(), &AbstractChatCompletionStream::dataAdded, this,
            [str](auto token) { *str += token; });

    connect(m_currentCompletion.get(), &AbstractChatCompletionStream::errorOccurred, this,
            [str, p = promise](const std::string &text) {
              qDebug() << "failed to run completion" << text;
              p->addResult(text);
              p->finish();
              delete str;
            });
    connect(m_currentCompletion.get(), &AbstractChatCompletionStream::finished, this, [str, p = promise]() {
      p->addResult(*str);
      p->finish();
      delete str;
    });

    return future;
  }

  QFuture<AI::Result<TranscriptionResponse>> transcribe(Audio::Recording recording, const QString &mime) {
    for (const auto &[id, provider] : m_providers) {
      if (const auto model = provider->findBestModel(Capability::Transcription)) {
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

  const auto &providers() const { return m_providers; }

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

  std::vector<AI::ProviderModel> modelsForProvider(std::string_view providerId) {
    auto it = m_providers.find(std::string(providerId));
    if (it == m_providers.end()) return {};

    std::vector<AI::ProviderModel> models;
    for (auto &model : it->second->listModels()) {
      models.emplace_back(std::string(providerId), std::move(model));
    }
    return models;
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
        return provider->createChatCompletion(model->id, payload);
      }
    }

    return nullptr;
  }

  static std::unique_ptr<AbstractProvider> createProvider(const std::string &id, std::string_view type);
  ProviderFields resolveFields(std::string_view id, const AbstractProvider &provider) const;
  void instantiate(const std::string &id, std::string_view type);
  void reconcile(const config::ConfigValue &current, const config::ConfigValue &previous);

  config::Manager &m_config;
  LocalStorageService &m_storage;
  std::unordered_map<std::string, std::unique_ptr<AI::AbstractProvider>> m_providers;

  mutable std::shared_ptr<AbstractChatCompletionStream> m_currentCompletion;
};
}; // namespace AI
