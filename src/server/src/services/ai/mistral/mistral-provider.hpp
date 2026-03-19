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
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <system_error>

namespace AI {

class MistralProvider : public AI::AbstractProvider {
  struct ListModelsResponse {
    struct ModelInfo {
      std::string id;
      std::string object; // model
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
    StandardChatCompletionPayload p;

    p.model = modelId;
    p.messages = payload.messages;

    return std::shared_ptr<StandardChatCompletionStream>(new StandardChatCompletionStream(m_client, p),
                                                         QObjectDeleter{});
  }

  std::string id() const override { return "mistral"; }

  std::optional<ImageUrl> icon() const override { return ImageUrl{BuiltinIcon::Mistral}; }

  std::string_view description() const override {
    return "Mistral AI cloud API. Provides transcription and language models.";
  }

  bool allowMultiple() const override { return false; }

  void start() override {
    listModels().then([this](const Result<ListModelsResponse> &res) {
      if (!res) {
        qWarning() << "Failed to fetch model list from mistral.ai" << res.error();
        return;
      }

      m_models.reserve(res->data.size());

      for (const auto &model : res->data) {
        auto m = transformModel(model);

        if (m.caps == 0) continue;

        m_models.emplace_back(transformModel(model));
      }
    });
  }

  QFuture<Result<ListModelsResponse>> listModels() { return m_client.get<ListModelsResponse>("/models"); }

  static Model transformModel(const ListModelsResponse::ModelInfo &info) {
    Model model;
    model.id = info.id;
    model.name = info.name;
    model.description = info.description;

    if (info.capabilities.completion_chat) { model.caps |= Capability::Completion; }
    if (info.capabilities.vision) { model.caps |= Capability::Vision; }
    if (info.capabilities.function_calling) { model.caps |= Capability::ToolCalling; }
    if (info.capabilities.audio_transcription) { model.caps |= Capability::Transcription; }

    return model;
  }

  std::optional<Model> findBestModel(Capabilities caps,
                                     Preference preference = Preference::None) const override {
    for (const auto &model : m_models) {
      if (model.caps & caps) return model;
    }

    return std::nullopt;
  }

  ModelList listModels(const ListModelFilters &filters = {}) const override { return m_models; }

  std::optional<Model> findModel(std::string_view id) {
    for (const auto &model : m_models)
      if (model.id == id) return model;
    return std::nullopt;
  }

  QFuture<TranscriptionResult> transcribe(const std::filesystem::path &path,
                                          const TranscriptionOptions &opts) override {
    std::error_code ec;

    if (!std::filesystem::is_regular_file(path, ec)) {
      qWarning() << path << "is not a valid file";
      return QtFuture::makeReadyValueFuture<TranscriptionResult>(
          std::unexpected(std::format("{} is not a regular file", path.c_str())));
    }

    auto file = new QFile(path.c_str());

    if (!file->open(QIODevice::ReadOnly)) {
      return QtFuture::makeReadyValueFuture<TranscriptionResult>(std::unexpected(
          std::format("Failed to open {}: {}", path.c_str(), file->errorString().toStdString())));
    }

    auto formData = new http::FormData;

    formData->addField("model", "voxtral-mini-2507");
    auto ext = path.extension().string();
    auto contentType = (ext == ".wav")                    ? "audio/wav"
                       : (ext == ".mp4" || ext == ".m4a") ? "audio/mp4"
                                                          : "audio/mpeg";
    formData->addFile(file, contentType);

    return m_client.post<TranscriptionResponse>("/audio/transcriptions", formData)
        .then([](AI::Result<TranscriptionResponse> res) -> TranscriptionResult {
          if (!res) { return std::unexpected(std::format("Transcription failed: {}", res.error())); }
          return TranscriptionResult(res->text);
        });
  }

public:
  MistralProvider(QString apiKey) {
    qDebug() << "mistral provider initialized with api key" << apiKey;
    m_client.setBaseUrl("https://api.mistral.ai/v1/");
    m_client.setBearer(std::move(apiKey));
  }

private:
  http::Client m_client;
  std::vector<Model> m_models;
};
}; // namespace AI
