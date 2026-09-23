#pragma once

#include <QElapsedTimer>
#include <QFutureWatcher>
#include <chrono>
#include <functional>
#include "ai-provider.hpp"

namespace AI {

class Agent : public QObject {
  Q_OBJECT

signals:
  void messageAdded(quint64 id);
  void textAdded(quint64 messageId, const std::string &text);
  void toolAdded(quint64 id);
  void toolChanged(quint64 id);
  void stateChanged();
  void finished();

public:
  using Id = quint64;
  using CompletionFactory = std::function<std::shared_ptr<AbstractChatCompletionStream>(
      const std::optional<ModelRef> &, const ChatCompletionPayload &)>;

  enum class State { Idle, Running, Completed, Failed, Cancelled };
  enum class ToolState { Queued, Running, Succeeded, Failed, Cancelled };

  struct Message {
    Id id;
    ChatMessage content;
  };
  struct ToolCall {
    Id id;
    Id messageId;
    ToolCallPart call;
    std::optional<std::string> summary;
    ToolState state = ToolState::Queued;
    std::optional<ToolOutput> output;
    std::optional<std::chrono::milliseconds> duration;
  };
  struct Options {
    std::optional<ModelRef> model;
    std::vector<std::string> tools;
    ThinkingMode thinking = ThinkingMode::Medium;
    int maxSteps = 16;
  };

  explicit Agent(CompletionFactory factory, ChatHistory history = {}, QObject *parent = nullptr);
  ~Agent() override;

  bool addTool(std::unique_ptr<AbstractTool> tool);
  bool send(ChatMessage message, Options options);
  void cancel();

  State state() const { return m_state; }
  bool running() const { return m_state == State::Running; }
  const std::optional<std::string> &error() const { return m_error; }
  const std::optional<Model> &model() const { return m_model; }
  const std::vector<Message> &messages() const { return m_messages; }
  const std::vector<ToolCall> &toolCalls() const { return m_calls; }
  const Message *message(Id id) const;
  const ToolCall *toolCall(Id id) const;

private:
  Id appendMessage(ChatMessage message);
  Message &assistantMessage();
  void complete();
  void receiveText(const std::string &text);
  void receiveToolCall(const ToolCallPart &call);
  void executeNextTool();
  void recordToolResult(Id id, ToolState state, ToolOutput output);
  void finish(State state, std::optional<std::string> error = {});
  void stopWork();
  void schedule(std::function<void()> action);

  CompletionFactory m_factory;
  std::vector<Message> m_messages;
  std::vector<ToolCall> m_calls;
  std::vector<std::unique_ptr<AbstractTool>> m_tools;
  std::shared_ptr<AbstractChatCompletionStream> m_completion;
  QFutureWatcher<AbstractTool::RawToolResult> *m_toolWatcher = nullptr;
  std::function<void()> m_cancelTool;
  QElapsedTimer m_toolTimer;
  std::vector<Id> m_pendingCalls;
  std::size_t m_nextCall = 0;
  std::optional<Id> m_assistantMessage;
  Options m_options;
  State m_state = State::Idle;
  std::optional<std::string> m_error;
  std::optional<Model> m_model;
  Id m_nextId = 1;
  quint64 m_generation = 0;
  int m_steps = 0;
  bool m_finishing = false;
};

} // namespace AI
