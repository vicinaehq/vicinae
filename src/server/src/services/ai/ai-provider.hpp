#pragma once
#include "ai-tool.hpp"
#include <cstdint>
#include <expected>
#include <glaze/core/common.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/prettify.hpp>
#include <optional>
#include <qdir.h>
#include <qfuture.h>
#include <qimage.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <ranges>
#include <string>
#include <vector>
#include "common/context.hpp"
#include "common/qt.hpp"
#include "http-client.hpp"
#include "image-url.hpp"

class AbstractTool;

namespace AI {

template <typename T> using Result = std::expected<T, std::string>;

/**
 * What a model is capable of.
 * Some models can do many 2026-03-05T21things at once.
 */
enum Capability : std::uint8_t {
  Completion = 1 << 0,
  Vision = 1 << 1,
  Thinking = 1 << 2,
  ToolCalling = 1 << 3,
  Embedding = 1 << 4,
  Transcription = 1 << 5,
  OCR = 1 << 6
};

using Capabilities = std::uint32_t;

struct Model {
  std::string id;
  std::string name;
  std::optional<std::string> description;
  std::optional<ImageUrl> icon;
  Capabilities caps = 0;
};

struct ModelRef {
  static std::expected<ModelRef, std::string> fromString(std::string_view model) {
    auto pos = model.find(':');

    if (pos == std::string::npos) {
      return std::unexpected("Expected at least one ':' to separate provider from model id");
    }

    return ModelRef(std::string{model.substr(0, pos)}, std::string{model.substr(pos + 1)});
  }

  std::string toString() const {
    std::string s;
    s.reserve(provider.size() + id.size() + 1);
    return s.append(provider).append(":").append(id);
  }

  std::string provider;
  std::string id;
};

struct ProviderModel : public Model {
  ModelRef ref;
  bool enabled = true;

  ProviderModel(std::string providerId, Model &&model)
      : Model(std::move(model)), ref{std::move(providerId), this->id} {}
};

inline std::vector<std::string> stringifyCapabilities(Capabilities caps) {
  std::vector<std::string> strs;

  if (caps & Completion) strs.emplace_back("completion");
  if (caps & Vision) strs.emplace_back("vision");
  if (caps & Thinking) strs.emplace_back("thinking");
  if (caps & ToolCalling) strs.emplace_back("tools");
  if (caps & Embedding) strs.emplace_back("embedding");
  if (caps & Transcription) strs.emplace_back("transcription");

  return strs;
}

/**
 * Indicates what kind of model is preferred for the current task at hand.
 * This is generally most about speed/cost.
 * Providers may ignore this completely.
 */
enum class Preference { None, Fast, Reasoning };

using ModelList = std::vector<Model>;

struct ListModelFilters {
  std::optional<Capabilities> caps;
  std::optional<int> limit;
};

class AbstractChatCompletionStream : public QObject {
  Q_OBJECT

public:
  struct ToolCallRequest {
    std::string id;
    std::string type;
    struct {
      std::string name;
      glz::raw_json arguments;
    } function;
  };

signals:
  void toolCallRequested(const ToolCallRequest &request) const;
  void dataAdded(const std::string &text) const;
  void errorOccured(const std::string &reason) const;
  void finished() const;

public:
  ~AbstractChatCompletionStream() override = default;
  virtual bool start() = 0;
  virtual bool abort() = 0;

  const Model &model() const { return m_model; }

protected:
  void setModel(Model model) { m_model = std::move(model); }

private:
  Model m_model;
};

enum class ChatRole { System, User, Assistant, Tool, Developer };

struct ChatMessage {
  ChatRole role;
  std::string value;
};

using ChatHistory = std::vector<ChatMessage>;

enum class ThinkingMode { None, Low, Medium, High };

struct ChatCompletionPayload {
  /**
   * How much thinking the model should do before answering, if applicable to the selected model.
   * Some providers may not support this at all.
   */
  ThinkingMode thinking = ThinkingMode::Medium;
  ChatHistory messages;
  std::optional<float> temperature;
  std::vector<AbstractTool *> tools;
};

struct TranscriptionOptions {
  std::string mime;
};

struct TranscriptionResponse {
  std::string text;
};

using TranscriptionResult = std::expected<TranscriptionResponse, std::string>;

class AbstractProvider : public QObject {
  Q_OBJECT

signals:
  void modelsUpdated() const;

public:
  AbstractProvider() = default;
  ~AbstractProvider() override = default;

  /**
   * Unique identifier for this provider.
   */
  virtual std::string id() const = 0;

  /**
   * An icon representing the provider, if applicable.
   */
  virtual std::optional<ImageUrl> icon() const = 0;

  /**
   * A human-readable description of this provider.
   */
  virtual std::string_view description() const = 0;

  virtual void start() = 0;

  /**
   * List all models that match the filter's criterias.
   */
  virtual ModelList listModels(const ListModelFilters &filters = {}) const = 0;

  /**
   * Find the best model for the provided set of capabilities and, if relevant, factor in the assigned task
   * preference.
   */
  virtual std::optional<Model> findBestModel(Capabilities caps,
                                             Preference preference = Preference::None) const = 0;

  /**
   * Create a streaming chat completion that can be started by calling the `start` method.
   * Streaming completions will continously emit the `dataAdded` signal with the new data until there is no
   * more data available, in which case `finished` is emitted. The completion can be aborted at anytime by
   * calling `abort`.
   */
  virtual std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(std::string_view modelId, const ChatCompletionPayload &payload) = 0;

  virtual QFuture<TranscriptionResult> transcribe(QIODevice *device,
                                                  const TranscriptionOptions &opts = {}) = 0;
};

struct StandardChatCompletionPayload {
  std::string model;
  AI::ChatHistory messages;
  std::vector<AbstractTool *> tools;
};

struct StandardChatCompletionRawPayload {
  struct Message {
    std::string role;
    std::string content;

    static std::string_view stringifyRole(AI::ChatRole role) {
      switch (role) {
      case AI::ChatRole::Assistant:
        return "assistant";
      case AI::ChatRole::System:
        return "system";
      case AI::ChatRole::User:
        return "user";
      case AI::ChatRole::Developer:
        return "developer";
      case AI::ChatRole::Tool:
        return "tool";
      }
    }

    static Message fromMessage(const AI::ChatMessage &message) {
      return Message(std::string{stringifyRole(message.role)}, message.value);
    }
  };

  static StandardChatCompletionRawPayload fromTypedPayload(const StandardChatCompletionPayload &payload) {
    StandardChatCompletionRawPayload raw{};

    raw.model = payload.model;
    raw.messages.reserve(payload.messages.size());
    raw.tools = payload.tools | std::views::transform([](AbstractTool *tool) { return tool->toolSchema(); }) |
                std::ranges::to<std::vector>();

    for (const auto &p : payload.messages) {
      raw.messages.emplace_back(StandardChatCompletionRawPayload::Message::fromMessage(p));
    }
    return raw;
  }

  std::string model;
  std::vector<Message> messages;
  std::vector<AbstractTool::ToolSchema> tools;
  bool stream = true;
};

struct StandardChatCompletionStreamChunk {
  struct ToolCall {
    std::string id;
    struct {
      std::string name;
      std::string arguments;
    } function;
    int index;
  };

  struct Choice {
    int index;
    struct {
      std::optional<std::string> role;
      std::optional<std::string> content;
      std::optional<std::vector<ToolCall>> tool_calls;
    } delta;
    std::optional<std::string> finish_reason;
  };

  std::string id;
  std::string object;
  std::int64_t created;
  std::string model;
  std::vector<Choice> choices;
};

class StandardChatCompletionStream : public AbstractChatCompletionStream {
public:
  static std::shared_ptr<StandardChatCompletionStream>
  makeShared(http::Client client, const StandardChatCompletionPayload &payload) {
    return std::shared_ptr<StandardChatCompletionStream>(
        new StandardChatCompletionStream(std::move(client), payload), QObjectDeleter{});
  }

  StandardChatCompletionStream(http::Client client, const StandardChatCompletionPayload &payload)
      : m_client(std::move(client)), m_payload(StandardChatCompletionRawPayload::fromTypedPayload(payload)) {}

  bool start() override {
    m_eventSource = m_client.postEventSource<StandardChatCompletionRawPayload>("chat/completions", m_payload);
    m_eventSource->setParent(this);
    connect(m_eventSource, &http::EventSource::dataReceived, this, &StandardChatCompletionStream::handleData);
    connect(m_eventSource, &http::EventSource::finished, this, &StandardChatCompletionStream::finished);
    connect(m_eventSource, &http::EventSource::errorOccured,
            [this](const QString &reason) { emit errorOccured(reason.toStdString()); });
    return true;
  }

  bool abort() override {
    if (m_eventSource) m_eventSource->abort();
    return true;
  }

private:
  void handleData(const QString &event, QByteArrayView data) {
    StandardChatCompletionStreamChunk chunk;

    if (data == "[DONE]") return;

    if (auto const error = glz::read<glz::opts{.error_on_unknown_keys = false}>(chunk, data.constData())) {
      qDebug() << "Failed to parse chat completion chunk" << glz::format_error(error);
      emit errorOccured(glz::format_error(error));
      return;
    }

    std::cout << glz::prettify_json(data.toByteArray().toStdString()) << std::endl;

    if (!chunk.choices.empty()) {
      auto &choice = chunk.choices.at(0);

      if (choice.finish_reason == "tool_calls" && choice.delta.tool_calls &&
          !choice.delta.tool_calls->empty()) {
        auto &call = choice.delta.tool_calls->at(0);
        emit toolCallRequested(
            ToolCallRequest{.id = call.id,
                            .type = "function",
                            .function = {.name = call.function.name, .arguments = call.function.arguments}});
      } else if (choice.delta.content) {
        emit dataAdded(choice.delta.content.value());
      }
    }
  }

  http::Client m_client;
  StandardChatCompletionRawPayload m_payload;
  http::EventSource *m_eventSource = nullptr;
};

}; // namespace AI
