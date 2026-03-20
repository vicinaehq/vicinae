#include "builtin_icon.hpp"
#include "common/qt.hpp"
#include "services/ai/ai-provider.hpp"
#include "lib/http-client.hpp"
#include <cstdint>
#include <filesystem>
#include <format>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <qbitarray.h>
#include <qcborvalue.h>
#include <qcontainerfwd.h>
#include <qdir.h>
#include <qhttpmultipart.h>
#include <qimage.h>
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <ranges>
#include <system_error>
#include <x86_64-pc-linux-gnu/gmp.h>

namespace AI {

class MistralProvider : public AI::AbstractProvider {
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

  struct CompletionRequest {
    struct Message {
      std::string content;
      std::string role;
    };
    std::string model;
    std::vector<Message> messages;
  };

  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(std::string_view modelId, const ChatCompletionPayload &payload) override {
    qDebug() << "model ID" << modelId;
    StandardChatCompletionPayload p;
    p.model = modelId;
    p.messages = payload.messages;
    p.tools = payload.tools;
    return StandardChatCompletionStream::makeShared(m_client, p);
  }

  std::string id() const override { return "mistral"; }

  std::optional<ImageUrl> icon() const override { return ImageUrl{BuiltinIcon::Mistral}; }

  std::string_view description() const override {
    return "Mistral AI cloud API. Provides transcription and language models.";
  }

  void start() override { m_listWatcher.setFuture(fetchModels()); }

  QFuture<Result<ListModelsResponse>> fetchModels() { return m_client.get<ListModelsResponse>("/models"); }

  static Model transformModel(const ListModelsResponse::ModelInfo &info) {
    Model model;
    model.id = info.id;
    model.name = info.name;
    model.description = info.description;

    if (info.capabilities.completion_chat) { model.caps |= Capability::Completion; }
    if (info.capabilities.vision) { model.caps |= Capability::Vision; }
    if (info.capabilities.function_calling) { model.caps |= Capability::ToolCalling; }
    if (info.capabilities.audio_transcription) { model.caps |= Capability::Transcription; }
    if (info.capabilities.ocr) { model.caps |= Capability::OCR; }

    return model;
  }

public:
  std::optional<Model> findBestModel(Capabilities caps,
                                     Preference preference = Preference::None) const override {
    for (const auto &model : m_models.data) {
      auto m = transformModel(model);
      if (m.caps & caps) return m;
    }

    return std::nullopt;
  }

  ModelList listModels(const ListModelFilters &filters = {}) const override {
    return m_models.data | std::views::transform(transformModel) |
           std::views::filter([](auto &&m) { return m.id == m.name && m.caps; }) |
           std::ranges::to<ModelList>();
  }

  std::optional<Model> findModel(std::string_view id) {
    for (const auto &model : listModels())
      if (model.id == id) return model;
    return std::nullopt;
  }

  QFuture<TranscriptionResult> transcribe(QIODevice *device, const TranscriptionOptions &opts) override {
    auto formData = new http::FormData;

    formData->addField("model", "voxtral-mini-2507");
    formData->addFile(device, opts.mime.c_str());

    return m_client.post<TranscriptionResponse>("/audio/transcriptions", formData)
        .then([](AI::Result<TranscriptionResponse> res) -> TranscriptionResult {
          if (!res) { return std::unexpected(std::format("Transcription failed: {}", res.error())); }
          return TranscriptionResult(res->text);
        });
  }

  MistralProvider(QString apiKey) {
    qDebug() << "mistral provider initialized with api key" << apiKey;
    m_client.setBaseUrl("https://api.mistral.ai/v1/");
    m_client.setBearer(std::move(apiKey));
    connect(&m_listWatcher, &decltype(m_listWatcher)::finished, this, &MistralProvider::handleListResult);
  }

private:
  void handleListResult() {
    if (m_listWatcher.isCanceled()) return;

    auto res = m_listWatcher.result();
    if (!res) {
      qWarning() << "Failed to fetch model list from mistral.ai" << res.error();
      return;
    }

    m_models = res.value();
  }

  http::Client::Watcher<AI::Result<ListModelsResponse>> m_listWatcher;
  http::Client m_client;
  ListModelsResponse m_models;
};
}; // namespace AI
