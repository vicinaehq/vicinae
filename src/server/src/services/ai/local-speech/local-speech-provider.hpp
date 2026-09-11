#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <qfuture.h>
#include "services/ai/ai-provider.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/local-speech-model-registry/local-speech-model-registry.hpp"
#include "ui/image/image-url.hpp"
#include "ui/image/url.hpp"

namespace AI {

/**
 * Exposes installed local speech models as transcription models.
 * Always present, no configuration. Transcription itself is not wired yet.
 */
class LocalSpeechProvider : public AbstractProvider {
public:
  static constexpr std::string_view ID = "local-speech";

  explicit LocalSpeechProvider(LocalSpeechModelRegistry &registry) : m_registry(registry) {}

  std::string id() const override { return std::string(ID); }

  std::optional<ImageUrl> icon() const override {
    return ImageUrl{ImageURL::builtin(BuiltinIcon::Microphone)};
  }

  std::string_view description() const override { return "Speech models installed on this machine."; }

  void start() override {
    connect(&m_registry, &LocalSpeechModelRegistry::modelsChanged, this, &AbstractProvider::modelsUpdated);
  }

  ModelList listModels(const ListModelFilters &filters = {}) const override {
    if (filters.caps && !(*filters.caps & Capability::Transcription)) return {};

    ModelList models;
    const auto available = m_registry.models();
    models.reserve(available.size());

    for (const auto &model : available) {
      if (!model.installed || model.info.engine == SpeechEngine::Vad) continue;
      models.emplace_back(toModel(model.info));
    }

    return models;
  }

  // Deliberately never chosen as a fallback until local transcription is implemented.
  std::optional<Model> findBestModel(Capabilities, Preference = Preference::None) const override {
    return std::nullopt;
  }

  std::shared_ptr<AbstractChatCompletionStream> createChatCompletion(std::string_view,
                                                                     const ChatCompletionPayload &) override {
    return nullptr;
  }

  QFuture<TranscriptionResult> transcribe(QIODevice *, const TranscriptionOptions & = {}) override {
    return QtFuture::makeReadyValueFuture<TranscriptionResult>(
        std::unexpected("Local transcription is not available yet"));
  }

private:
  static Model toModel(const SpeechModelInfo &info) {
    return Model{
        .id = std::string(info.id),
        .name = std::string(info.name),
        .description = SpeechModelCatalogue::translatedDescription(info).toStdString(),
        .icon = ImageUrl{SpeechModelCatalogue::vendorIcon(info.vendor)},
        .caps = Capability::Transcription,
    };
  }

  LocalSpeechModelRegistry &m_registry;
};

} // namespace AI
