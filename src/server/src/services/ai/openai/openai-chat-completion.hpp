#pragma once
#include <format>
#include <glaze/core/opts.hpp>
#include <glaze/json/read.hpp>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>
#include <qlogging.h>
#include "common/qt.hpp"
#include "internal/http-client.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/ai/ai-tool.hpp"

namespace AI::openai {

struct ContentPart {
  struct ImageUrl {
    std::string url;
  };

  std::string type;
  std::optional<std::string> text;
  std::optional<ImageUrl> image_url;
};

struct ToolCall {
  struct Function {
    std::string name;
    std::string arguments;
  };

  std::string id;
  std::string type{"function"};
  Function function;
};

struct Message {
  std::string role;
  std::optional<std::variant<std::string, std::vector<ContentPart>>> content;
  std::optional<std::vector<ToolCall>> tool_calls;
  std::optional<std::string> tool_call_id;

  static Message fromMessage(const ChatMessage &message) {
    Message out{.role = std::string{roleName(message.role)}};
    std::vector<ContentPart> parts;
    std::string text;
    bool hasImage = false;

    for (const auto &part : message.parts) {
      if (const auto *p = std::get_if<TextPart>(&part)) {
        parts.emplace_back(ContentPart{.type = "text", .text = p->text});
        text += p->text;
      } else if (const auto *p = std::get_if<ImagePart>(&part)) {
        parts.emplace_back(ContentPart{
            .type = "image_url",
            .image_url = ContentPart::ImageUrl{std::format("data:{};base64,{}", p->mimeType, p->base64)}});
        hasImage = true;
      } else if (const auto *p = std::get_if<ToolCallPart>(&part)) {
        if (!out.tool_calls) out.tool_calls.emplace();
        out.tool_calls->emplace_back(ToolCall{.id = p->id, .function = {p->name, p->arguments}});
      } else if (const auto *p = std::get_if<ToolResultPart>(&part)) {
        out.tool_call_id = p->callId;
        text += p->content;
      }
    }

    if (hasImage) {
      out.content = std::move(parts);
    } else if (!text.empty() || !out.tool_calls) {
      out.content = std::move(text);
    }

    return out;
  }
};

struct ChatRequest {
  std::string model;
  std::vector<Message> messages;
  std::optional<std::vector<AbstractTool::ToolSchema>> tools;
  std::optional<float> temperature;
  std::optional<std::string> reasoning_effort;
  bool stream = true;

  static ChatRequest make(std::string_view model, const ChatCompletionPayload &payload) {
    ChatRequest request{.model = std::string{model}, .temperature = payload.temperature};

    request.messages.reserve(payload.messages.size());
    for (const auto &message : payload.messages) {
      request.messages.emplace_back(Message::fromMessage(message));
    }

    if (!payload.tools.empty()) {
      request.tools = payload.tools |
                      std::views::transform([](AbstractTool *tool) { return tool->toolSchema(); }) |
                      std::ranges::to<std::vector>();
    }

    return request;
  }
};

struct StreamChunk {
  struct ToolCallDelta {
    struct Function {
      std::optional<std::string> name;
      std::optional<std::string> arguments;
    };

    int index = 0;
    std::optional<std::string> id;
    Function function;
  };

  struct Choice {
    struct Delta {
      std::optional<std::string> role;
      std::optional<std::string> content;
      std::optional<std::vector<ToolCallDelta>> tool_calls;
    };

    int index = 0;
    Delta delta;
    std::optional<std::string> finish_reason;
  };

  std::vector<Choice> choices;
};

class ChatCompletionStream : public AbstractChatCompletionStream {
public:
  static std::shared_ptr<ChatCompletionStream> makeShared(http::Client client, ChatRequest request,
                                                          Model model = {}) {
    return std::shared_ptr<ChatCompletionStream>(
        new ChatCompletionStream(std::move(client), std::move(request), std::move(model)), QObjectDeleter{});
  }

  ChatCompletionStream(http::Client client, ChatRequest request, Model model)
      : m_client(std::move(client)), m_request(std::move(request)) {
    setModel(std::move(model));
  }

  bool start() override {
    m_eventSource = m_client.postEventSource("chat/completions", m_request);
    if (!m_eventSource) return false;

    m_eventSource->setParent(this);
    connect(m_eventSource, &http::EventSource::dataReceived, this, &ChatCompletionStream::handleData);
    connect(m_eventSource, &http::EventSource::finished, this, &ChatCompletionStream::finished);
    connect(m_eventSource, &http::EventSource::errorOccured, this,
            [this](const QString &reason) { emit errorOccured(reason.toStdString()); });
    return true;
  }

  bool abort() override {
    if (m_eventSource) m_eventSource->abort();
    return true;
  }

private:
  void handleData(const QString &, QByteArrayView data) {
    if (data.trimmed() == "[DONE]") return;

    StreamChunk chunk;
    const std::string json(data.data(), static_cast<std::size_t>(data.size()));

    if (auto const error = glz::read<glz::opts{.error_on_unknown_keys = false}>(chunk, json)) {
      qWarning() << "Failed to parse chat completion chunk" << glz::format_error(error, json);
      emit errorOccured(glz::format_error(error));
      return;
    }

    if (chunk.choices.empty()) return;

    const auto &choice = chunk.choices.front();

    if (choice.delta.content && !choice.delta.content->empty()) emit dataAdded(*choice.delta.content);

    if (choice.delta.tool_calls) {
      for (const auto &delta : *choice.delta.tool_calls) {
        auto &call = m_toolCalls[delta.index];
        if (delta.id) call.id = *delta.id;
        if (delta.function.name) call.name += *delta.function.name;
        if (delta.function.arguments) call.arguments += *delta.function.arguments;
      }
    }

    if (choice.finish_reason && !m_toolCalls.empty()) {
      for (auto &[index, call] : std::exchange(m_toolCalls, {})) {
        emit toolCallRequested(call);
      }
    }
  }

  http::Client m_client;
  ChatRequest m_request;
  http::EventSource *m_eventSource = nullptr;
  std::map<int, ToolCallPart> m_toolCalls;
};

} // namespace AI::openai
