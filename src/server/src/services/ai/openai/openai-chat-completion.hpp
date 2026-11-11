#pragma once
#include <algorithm>
#include <format>
#include <glaze/core/opts.hpp>
#include <glaze/json/generic.hpp>
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
#include <QUuid>
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

// Gemini's OpenAI-compatible endpoint carries its thought signatures here.
struct ExtraContent {
  struct Google {
    std::optional<std::string> thought_signature;
  };
  std::optional<Google> google;
};

struct ToolCall {
  struct Function {
    std::string name;
    std::string arguments;
  };

  std::string id;
  std::string type{"function"};
  Function function;
  std::optional<ExtraContent> extra_content;
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
        parts.emplace_back(ContentPart{.type = "image_url",
                                       .image_url = ContentPart::ImageUrl{std::format(
                                           "data:{};base64,{}", p->mimeType, p->base64.toStdString())}});
        hasImage = true;
      } else if (const auto *p = std::get_if<ToolCallPart>(&part)) {
        if (!out.tool_calls) out.tool_calls.emplace();
        ToolCall call{.id = p->id, .function = {p->name, p->arguments}};
        if (p->thought_signature) {
          call.extra_content = ExtraContent{ExtraContent::Google{.thought_signature = p->thought_signature}};
        }
        out.tool_calls->emplace_back(std::move(call));
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

struct StreamOptions {
  bool include_usage = true;
};

struct ChatRequest {
  std::string model;
  std::vector<Message> messages;
  std::optional<std::vector<AbstractTool::ToolSchema>> tools;
  std::optional<std::string> reasoning_effort;
  std::optional<int> max_tokens;
  std::optional<int> max_completion_tokens;
  std::optional<StreamOptions> stream_options;
  bool stream = true;

  static ChatRequest make(std::string_view model, const ChatCompletionPayload &payload) {
    ChatRequest request{.model = std::string{model}, .max_tokens = payload.maxOutputTokens};

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
  struct Usage {
    struct PromptDetails {
      std::optional<int> cached_tokens;
    };
    std::optional<int> prompt_tokens;
    std::optional<int> completion_tokens;
    std::optional<PromptDetails> prompt_tokens_details;
  };

  std::optional<Usage> usage;

  struct ReasoningDetail {
    std::string type;
  };

  struct ToolCallDelta {
    struct Function {
      std::optional<std::string> name;
      std::optional<std::string> arguments;
    };

    std::optional<int> index;
    std::optional<std::string> id;
    std::optional<std::string> thought_signature;
    std::optional<ExtraContent> extra_content;
    Function function;
  };

  // reasoning models such as mistral-large-4 stream typed parts instead of plain text
  struct DeltaContentPart {
    std::string type;
    std::optional<std::string> text;
  };

  struct Choice {
    struct Delta {
      std::optional<std::string> role;
      std::optional<std::variant<std::string, std::vector<DeltaContentPart>>> content;
      std::optional<std::string> reasoning;
      std::optional<std::string> reasoning_content;
      std::optional<std::vector<ReasoningDetail>> reasoning_details;
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
    connect(m_eventSource, &http::EventSource::errorOccurred, this,
            [this](const QString &reason, const QByteArray &body) {
              emit errorOccurred(describeError(reason.toStdString(), body));
            });
    return true;
  }

  bool abort() override {
    if (m_eventSource) m_eventSource->abort();
    return true;
  }

private:
  static std::string describeError(std::string reason, const QByteArray &body) {
    glz::generic payload;
    const std::string json(body.constData(), static_cast<std::size_t>(body.size()));

    if (!glz::read_json(payload, json) && payload.is_object()) {
      const auto &object = payload.get_object();
      if (auto it = object.find("error"); it != object.end()) {
        if (it->second.is_string()) return it->second.get_string();
        if (it->second.is_object()) {
          const auto &error = it->second.get_object();
          if (auto message = error.find("message"); message != error.end() && message->second.is_string()) {
            return message->second.get_string();
          }
        }
      }
    }

    if (!body.trimmed().isEmpty()) reason.append(": ").append(body.trimmed().constData());
    return reason;
  }

  // Some servers (Gemini) omit the index and send every call whole, sometimes with an empty id.
  // A delta carrying a function name then starts a new call.
  int resolveToolIndex(const StreamChunk::ToolCallDelta &delta) const {
    if (delta.index) return *delta.index;
    if (m_toolCalls.empty()) return 0;
    if (delta.id && !delta.id->empty()) {
      const auto it =
          std::ranges::find_if(m_toolCalls, [&](const auto &entry) { return entry.second.id == *delta.id; });
      if (it != m_toolCalls.end()) return it->first;
    }
    const auto last = m_toolCalls.rbegin()->first;
    const bool startsCall =
        (delta.id && !delta.id->empty()) || (delta.function.name && !delta.function.name->empty());
    return startsCall ? last + 1 : last;
  }

  void handleData(const QString &, QByteArrayView data) {
    if (data.trimmed() == "[DONE]") return;

    StreamChunk chunk;
    const std::string json(data.data(), static_cast<std::size_t>(data.size()));

    if (auto const error = glz::read<glz::opts{.error_on_unknown_keys = false}>(chunk, json)) {
      qWarning() << "Failed to parse chat completion chunk" << glz::format_error(error, json);
      emit errorOccurred(glz::format_error(error));
      return;
    }

    if (chunk.usage) {
      const auto &usage = *chunk.usage;
      const auto cached =
          usage.prompt_tokens_details ? usage.prompt_tokens_details->cached_tokens : std::nullopt;
      qInfo().nospace() << "AI usage: model=" << m_request.model
                        << " input=" << usage.prompt_tokens.value_or(0) << " cached=" << cached.value_or(0)
                        << " output=" << usage.completion_tokens.value_or(0);
    }

    if (chunk.choices.empty()) return;

    const auto &choice = chunk.choices.front();

    const auto &delta = choice.delta;
    if ((delta.reasoning && !delta.reasoning->empty()) ||
        (delta.reasoning_content && !delta.reasoning_content->empty()) ||
        (delta.reasoning_details && !delta.reasoning_details->empty())) {
      emit activityChanged(ResponseActivity::Thinking);
    }

    if (delta.content) handleContent(*delta.content);

    if (delta.tool_calls && !delta.tool_calls->empty()) {
      emit activityChanged(ResponseActivity::PreparingTool);
      for (const auto &toolDelta : *delta.tool_calls) {
        auto &call = m_toolCalls[resolveToolIndex(toolDelta)];
        if (toolDelta.id && !toolDelta.id->empty()) call.id = *toolDelta.id;
        if (toolDelta.thought_signature) call.thought_signature = *toolDelta.thought_signature;
        if (toolDelta.extra_content && toolDelta.extra_content->google &&
            toolDelta.extra_content->google->thought_signature) {
          call.thought_signature = *toolDelta.extra_content->google->thought_signature;
        }
        if (toolDelta.function.name) call.name += *toolDelta.function.name;
        if (toolDelta.function.arguments) call.arguments += *toolDelta.function.arguments;
      }
    }

    if (choice.finish_reason && !m_toolCalls.empty()) {
      for (auto &[index, call] : std::exchange(m_toolCalls, {})) {
        if (call.id.empty())
          call.id = std::format("call_{}", QUuid::createUuid().toString(QUuid::Id128).toStdString());
        emit toolCallRequested(call);
      }
    }
  }

  void handleContent(const std::variant<std::string, std::vector<StreamChunk::DeltaContentPart>> &content) {
    if (const auto *text = std::get_if<std::string>(&content)) {
      if (!text->empty()) emit dataAdded(*text);
      return;
    }

    std::string text;
    bool thinking = false;
    for (const auto &part : std::get<std::vector<StreamChunk::DeltaContentPart>>(content)) {
      if (part.type == "thinking") {
        thinking = true;
      } else if (part.type == "text" && part.text) {
        text += *part.text;
      }
    }

    if (thinking) emit activityChanged(ResponseActivity::Thinking);
    if (!text.empty()) emit dataAdded(text);
  }

  http::Client m_client;
  ChatRequest m_request;
  http::EventSource *m_eventSource = nullptr;
  std::map<int, ToolCallPart> m_toolCalls;
};

} // namespace AI::openai
