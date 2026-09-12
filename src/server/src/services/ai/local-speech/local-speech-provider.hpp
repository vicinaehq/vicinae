#pragma once
#include <optional>
#include <qlogging.h>
#include <qtconcurrentrun.h>
#include <string>
#include <string_view>
#include <qfuture.h>
#include "parakeet.h"
#include "services/ai/ai-provider.hpp"
#include "services/audio/audio-recorder.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/local-speech-model-registry/local-speech-model-registry.hpp"
#include "services/local-speech-model-registry/speech-model-catalogue.hpp"
#include "ui/image/image-url.hpp"
#include "ui/image/url.hpp"
#include "whisper.h"

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

  QFuture<TranscriptionResult> transcribe(Audio::Recording recording,
                                          const TranscriptionOptions &opts = {}) override {
    if (!opts.model)
      return QtFuture::makeReadyValueFuture<TranscriptionResult>(std::unexpected("No model was specified"));

    auto model = m_registry.model(*opts.model);

    if (!model) {
      return QtFuture::makeReadyValueFuture<TranscriptionResult>(std::unexpected("Model could not be found"));
    }

    auto path = m_registry.pathFor(model->info);

    // TODO: move this in its own process in order to avoid crashing Vicinae if for some reason
    // whisper crashes. Also, we need to keep the context alive in order to avoid cold starts every time
    // like it is the case right now. But we don't want to keep it initialized at all times, as it can be
    // withold a lot of resources.

    const auto runParakeet = [&]() {
      return QtConcurrent::run(
          [recording = std::move(recording), opts, path = std::move(path)]() -> TranscriptionResult {
            parakeet_full_params fparams =
                parakeet_full_default_params(parakeet_sampling_strategy::PARAKEET_SAMPLING_GREEDY);
            auto ctx =
                parakeet_init_from_file_with_params(path.string().c_str(), parakeet_context_default_params());

            qDebug() << "transcribing using whisper full, model" << path;

            if (parakeet_full(ctx, fparams, recording.toF32().data(), recording.toF32().size()) != 0) {
              return std::unexpected("Failed to transcribe");
            }

            qDebug() << "Transcription is done.";

            const int n_segments = parakeet_full_n_segments(ctx);
            std::string text{};

            for (int i = 0; i < n_segments; ++i) {
              text += parakeet_full_get_segment_text(ctx, i);
            }

            parakeet_free(ctx);

            return TranscriptionResult{text};
          });
    };

    const auto runWhisper = [&]() {
      return QtConcurrent::run(
          [recording = std::move(recording), opts, path = std::move(path)]() -> TranscriptionResult {
            whisper_context_params params = whisper_context_default_params();
            params.use_gpu = opts.useGpu;

            whisper_context *ctx = whisper_init_from_file_with_params(path.string().c_str(), params);
            whisper_full_params fparams =
                whisper_full_default_params(whisper_sampling_strategy::WHISPER_SAMPLING_GREEDY);
            fparams.language = opts.language ? opts.language->c_str() : "auto";

            qDebug() << "transcribing using whisper full, model" << path;

            if (whisper_full(ctx, fparams, recording.toF32().data(), recording.toF32().size()) != 0) {
              return std::unexpected("Failed to transcribe");
            }

            qDebug() << "Transcription is done.";

            const int n_segments = whisper_full_n_segments(ctx);
            std::string text{};

            for (int i = 0; i < n_segments; ++i) {
              text += whisper_full_get_segment_text(ctx, i);
            }

            whisper_free(ctx);

            return TranscriptionResult{text};
          });
    };

    switch (model->info.engine) {
    case SpeechEngine::Parakeet:
      return runParakeet();
    case SpeechEngine::Whisper:
      return runWhisper();
    default:
      return QtFuture::makeReadyValueFuture<TranscriptionResult>(
          std::unexpected("Cannot run inference for this engine"));
    }
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
