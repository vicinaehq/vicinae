#include "builtin_icon.hpp"
#include "preference.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/ai/json-client.hpp"
#include <filesystem>
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
  createChatCompletion(const ChatCompletionPayload &payload) override {
    return nullptr;
    /*
QUrl url("https://api.mistral.ai/v1/chat/completions");

m_client.post<>()

  return nullptr;
          */
  }

  std::string id() const override { return "mistral"; }

  std::optional<ImageUrl> icon() const override { return ImageUrl{BuiltinIcon::Mistral}; }

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

  QFuture<TranscriptionResult> transcribe(const std::filesystem::path &path) override {
    QUrl url("https://api.mistral.ai/v1/audio/transcriptions");

    std::error_code ec;

    if (!std::filesystem::is_regular_file(path, ec)) {
      qWarning() << path << "is not a valid file";
      return {};
    }

    auto file = new QFile(path.c_str());

    if (!file->open(QIODevice::ReadOnly)) {
      qWarning() << "Failed to open file" << path.c_str() << file->errorString();
    }

    auto formData = new FormData;

    formData->addField("model", "voxtral-mini-2507");
    formData->addFile(file, "video/mp4");

    return m_client.post<TranscriptionResponse>(url, formData)
        .then([](AI::Result<TranscriptionResponse> res) {
          if (!res) { qWarning() << "Transcription failed" << res.error(); }
          return TranscriptionResult(res->text);
        });
  }

public:
  MistralProvider(QString apiKey) {
    qDebug() << "mistral provider initialized with api key" << apiKey;
    m_client.setBearer(std::move(apiKey));
  }

private:
  JsonClient m_client;
};
}; // namespace AI
