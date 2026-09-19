#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <qfuture.h>
#include <QString>
#include "internal/http-client.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/audio/audio-recorder.hpp"
#include "ui/image/image-url.hpp"

namespace AI {

namespace mistral {

struct ListModelsResponse {
  struct ModelInfo {
    std::string id;
    std::string object; // 'model' in all instances.
    std::string name;
    std::string description;
    std::uint32_t max_context_length;

    struct {
      bool completion_chat;
      bool function_calling;
      bool completion_fim;
      bool fine_tuning;
      bool vision;
      bool ocr;
      bool classification;
      bool moderation;
      bool audio;
      bool audio_transcription;
      bool audio_transcription_realtime;
      bool audio_speech;
    } capabilities;
  };

  std::vector<ModelInfo> data;
};

struct TranscriptionResponse {
  struct Usage {
    int prompt_audio_seconds;
    int prompt_tokens;
    int total_tokens;
    int completion_tokens;
  };

  std::string model;
  std::string text;
  std::optional<std::string> language;
  std::vector<std::string> segments;
};

} // namespace mistral

class MistralProvider : public AbstractProvider {
public:
  MistralProvider();

  std::string id() const override { return "mistral"; }
  std::string_view type() const override { return "mistral"; }
  std::optional<ImageUrl> icon() const override;
  std::string_view description() const override {
    return "Mistral AI cloud API. Provides transcription and language models.";
  }

  void configure(const ProviderFields &fields) override;
  void start() override;

  ModelList listModels(const ListModelFilters &filters = {}) const override;
  std::optional<Model> findBestModel(Capabilities caps,
                                     Preference preference = Preference::None) const override;

  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(std::string_view modelId, const ChatCompletionPayload &payload) override;
  QFuture<TranscriptionResult> transcribe(Audio::Recording recording,
                                          const TranscriptionOptions &opts) override;

private:
  QFuture<Result<mistral::ListModelsResponse>> fetchModels();
  void handleListResult();

  http::Client::Watcher<Result<mistral::ListModelsResponse>> m_listWatcher;
  http::Client m_client;
  mistral::ListModelsResponse m_models;
  QString m_apiKey;
  bool m_started = false;
};

} // namespace AI
