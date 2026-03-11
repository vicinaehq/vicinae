#include "builtin_icon.hpp"
#include "preference.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/ai/json-client.hpp"
#include <filesystem>
#include <format>
#include <qcborvalue.h>
#include <qdir.h>
#include <qhttpmultipart.h>
#include <qlogging.h>
#include <system_error>

namespace AI {

class MistralProvider : public AI::AbstractProvider {
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
    return nullptr;
  }

  std::string id() const override { return "mistral"; }

  std::optional<ImageUrl> icon() const override { return ImageUrl{BuiltinIcon::Mistral}; }

  std::string_view description() const override {
    return "Mistral AI cloud API. Provides transcription and language models.";
  }

  bool allowMultiple() const override { return false; }

  void start() override {}

  std::optional<Model> findBestModel(Capabilities caps,
                                     Preference preference = Preference::None) const override {
    if (caps == Capability::Transcription) {
      return AI::Model{.id = "voxtral-mini-latest", .name = "voxtral", .caps = Capability::Transcription};
    }
    return std::nullopt;
  }

  ModelList listModels(const ListModelFilters &filters = {}) const override {
    AI::Model voxtral{.id = "voxtral-mini-latest", .name = "voxtral", .caps = Capability::Transcription};

    return {voxtral};
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

    auto formData = new FormData;

    formData->addField("model", "voxtral-mini-2507");
    formData->addFile(file, "audio/mp3");

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
  JsonClient m_client;
};
}; // namespace AI
