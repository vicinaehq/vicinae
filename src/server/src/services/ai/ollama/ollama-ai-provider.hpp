#include "services/ai/ai-provider.hpp"
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include "services/ai/http-completion.hpp"
#include <glaze/json/write.hpp>
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qnetworkrequest.h>
#include <ranges>

namespace AI {

struct OllamaConfig {
  std::string url = "http://localhost:11434";
  std::optional<std::string> pwd;
};

class OllamaProvider : public AbstractProvider {

  struct ChatMessage {
    std::string role;
    std::string content;
  };

  struct ChatPayload {
    std::string model;
    std::vector<ChatMessage> messages;
    bool stream = true;
  };

  std::string id() const override { return "ollama"; }

  std::optional<ImageUrl> icon() const override { return {}; }

  ModelList listModels(const ListModelFilters &filters = {}) const override { return {}; }

  std::optional<Model> findBestModel(Capabilities caps,
                                     Preference preference = Preference::None) const override {
    return std::nullopt;
  }

  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(const ChatCompletionPayload &payload) const override {
    QNetworkRequest req;
    QUrl url = QString::fromStdString(m_cfg.url);

    url.setPath("/api/chat");
    req.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");

    auto serializeRole = [](AI::ChatRole role) {
      switch (role) {
      case AI::ChatRole::User:
        return "user";
      case AI::ChatRole::Assistant:
        return "assistant";
      case AI::ChatRole::System:
        return "system";
      }
      return "none";
    };

    req.setUrl(url);

    auto tr = [&](const AI::ChatMessage &msg) -> ChatMessage {
      return ChatMessage(serializeRole(msg.role), msg.value);
    };

    ChatPayload data;

    data.model = payload.modelId;
    data.messages = payload.messages | std::views::transform(tr) | std::ranges::to<std::vector>();

    std::string serializedData;

    if (const auto error = glz::write_json(data, serializedData)) {
      qWarning() << "Failed to serialize ollama payload" << glz::format_error(error);
      return nullptr;
    }

    qDebug() << url << serializedData;

    return std::make_shared<HttpCompletion>(req, QByteArray::fromStdString(serializedData));
  }

  QFuture<TranscriptionResult> transcribe() const override { return {}; }

  bool initalize(const glz::raw_json &json) override {
    if (const auto error = glz::read_json(m_cfg, json.str)) { return false; }
    return true;
  }

private:
  OllamaConfig m_cfg;
};

}; // namespace AI
