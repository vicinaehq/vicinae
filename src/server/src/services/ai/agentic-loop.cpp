#include <QTimer>
#include <algorithm>
#include <utility>

#include "agentic-loop.hpp"

namespace AI {

Agent::Agent(CompletionFactory factory, ChatHistory history, QObject *parent)
    : QObject(parent), m_factory(std::move(factory)) {
  m_messages.reserve(std::max<std::size_t>(16, history.size()));
  m_calls.reserve(8);
  m_tools.reserve(4);
  m_pendingCalls.reserve(4);

  for (auto &message : history)
    appendMessage(std::move(message));
}

Agent::~Agent() { stopWork(); }

void Agent::restoreHistory(std::vector<Message> messages, std::vector<ToolCall> calls) {
  Q_ASSERT(!running());
  m_messages = std::move(messages);
  m_calls = std::move(calls);
  m_nextId = 1;

  for (const auto &message : m_messages)
    m_nextId = std::max(m_nextId, message.id + 1);
  for (const auto &call : m_calls)
    m_nextId = std::max(m_nextId, call.id + 1);
}

bool Agent::addTool(std::unique_ptr<AbstractTool> tool) {
  if (running() || !tool ||
      std::ranges::any_of(m_tools, [&](const auto &existing) { return existing->name() == tool->name(); }))
    return false;

  m_tools.emplace_back(std::move(tool));
  return true;
}

const Agent::Message *Agent::message(Id id) const {
  const auto it = std::ranges::lower_bound(m_messages, id, {}, &Message::id);
  return it != m_messages.end() && it->id == id ? &*it : nullptr;
}

const Agent::ToolCall *Agent::toolCall(Id id) const {
  const auto it = std::ranges::lower_bound(m_calls, id, {}, &ToolCall::id);
  return it != m_calls.end() && it->id == id ? &*it : nullptr;
}

Agent::Id Agent::appendMessage(ChatMessage message) {
  const auto id = m_nextId++;
  if (m_messages.size() == m_messages.capacity()) m_messages.reserve(m_messages.size() * 2);
  m_messages.emplace_back(Message{id, std::move(message)});

  emit messageAdded(id);
  return id;
}

bool Agent::send(ChatMessage message, Options options) {
  if (running() || m_finishing || message.role != ChatRole::User || message.parts.empty() ||
      options.maxSteps < 1)
    return false;

  m_options = std::move(options);
  m_steps = 0;
  m_error.reset();
  m_model.reset();
  m_activity = ResponseActivity::Waiting;
  m_state = State::Running;
  const auto generation = ++m_generation;

  emit stateChanged();
  if (!running() || generation != m_generation) return true;

  appendMessage(std::move(message));
  if (!running() || generation != m_generation) return true;

  schedule([this] { complete(); });
  return true;
}

void Agent::schedule(std::function<void()> action) {
  QTimer::singleShot(0, this, [this, generation = m_generation, action = std::move(action)] {
    if (running() && generation == m_generation) action();
  });
}

Agent::Message &Agent::assistantMessage() {
  if (!m_assistantMessage) m_assistantMessage = appendMessage(ChatMessage{.role = ChatRole::Assistant});
  return *std::ranges::find(m_messages, *m_assistantMessage, &Message::id);
}

void Agent::complete() {
  if (m_steps++ >= m_options.maxSteps) {
    finish(State::Failed, tr("The agent reached its step limit.").toStdString());
    return;
  }

  m_assistantMessage.reset();
  m_pendingCalls.clear();
  m_nextCall = 0;

  const auto generation = m_generation;
  setActivity(ResponseActivity::Waiting);
  if (!running() || generation != m_generation) return;

  ChatCompletionPayload payload{.thinking = m_options.thinking};
  payload.messages.reserve(m_messages.size());
  for (const auto &message : m_messages)
    payload.messages.emplace_back(message.content);

  payload.tools.reserve(m_options.tools.size());
  for (const auto &tool : m_tools) {
    if (tool->isEnabled() && std::ranges::contains(m_options.tools, tool->name()))
      payload.tools.emplace_back(tool.get());
  }

  m_completion = m_factory(m_options.model, payload);
  if (!m_completion) {
    finish(State::Failed, tr("This model is not available right now.").toStdString());
    return;
  }

  m_model = m_completion->model();

  connect(m_completion.get(), &AbstractChatCompletionStream::activityChanged, this,
          [this, generation](ResponseActivity activity) {
            if (running() && generation == m_generation) setActivity(activity);
          });

  connect(m_completion.get(), &AbstractChatCompletionStream::dataAdded, this,
          [this, generation](const std::string &text) {
            if (running() && generation == m_generation) receiveText(text);
          });

  connect(m_completion.get(), &AbstractChatCompletionStream::toolCallRequested, this,
          [this, generation](const ToolCallPart &call) {
            if (running() && generation == m_generation) receiveToolCall(call);
          });

  connect(m_completion.get(), &AbstractChatCompletionStream::errorOccurred, this,
          [this, generation](const std::string &reason) {
            if (running() && generation == m_generation) finish(State::Failed, reason);
          });

  connect(m_completion.get(), &AbstractChatCompletionStream::finished, this, [this, generation] {
    if (!running() || generation != m_generation) return;

    disconnect(m_completion.get(), nullptr, this, nullptr);
    m_completion.reset();

    // A provider may emit several calls in one response. Execute only after the response is complete.
    schedule([this] { executeNextTool(); });
  });

  const auto completion = m_completion;
  if (!completion->start() && running() && generation == m_generation)
    finish(State::Failed, tr("Could not start the response.").toStdString());
}

void Agent::receiveText(const std::string &text) {
  if (text.empty()) return;

  const auto generation = m_generation;
  setActivity(ResponseActivity::Responding);
  if (!running() || generation != m_generation) return;
  auto &message = assistantMessage();
  if (!running() || generation != m_generation) return;

  auto &parts = message.content.parts;

  if (!parts.empty() && std::holds_alternative<TextPart>(parts.back()))
    std::get<TextPart>(parts.back()).text += text;
  else {
    parts.reserve(parts.size() + 1);
    parts.emplace_back(TextPart{text});
  }

  emit textAdded(message.id, text);
}

void Agent::receiveToolCall(const ToolCallPart &call) {
  if (call.id.empty() || call.name.empty() ||
      std::ranges::any_of(m_pendingCalls, [&](Id id) { return toolCall(id)->call.id == call.id; })) {
    finish(State::Failed, tr("The model returned an invalid tool call.").toStdString());
    return;
  }

  const auto generation = m_generation;
  setActivity(ResponseActivity::PreparingTool);
  if (!running() || generation != m_generation) return;
  auto &message = assistantMessage();
  if (!running() || generation != m_generation) return;

  message.content.parts.reserve(message.content.parts.size() + 1);
  message.content.parts.emplace_back(call);

  const auto id = m_nextId++;
  if (m_calls.size() == m_calls.capacity()) m_calls.reserve(m_calls.size() * 2);

  const auto tool =
      std::ranges::find_if(m_tools, [&](const auto &tool) { return tool->name() == call.name; });
  m_calls.emplace_back(
      ToolCall{.id = id,
               .messageId = message.id,
               .call = call,
               .summary = tool != m_tools.end() ? (*tool)->invocationSummary(call.arguments) : std::nullopt});
  m_pendingCalls.emplace_back(id);

  emit toolAdded(id);
}

void Agent::executeNextTool() {
  const auto generation = m_generation;

  if (m_nextCall == m_pendingCalls.size()) {
    if (m_pendingCalls.empty())
      finish(State::Completed);
    else
      schedule([this] { complete(); });
    return;
  }

  const auto id = m_pendingCalls[m_nextCall++];
  auto &call = *std::ranges::find(m_calls, id, &ToolCall::id);
  const auto tool = std::ranges::find_if(m_tools, [&](const auto &tool) {
    return tool->name() == call.call.name && tool->isEnabled() &&
           std::ranges::contains(m_options.tools, tool->name());
  });

  if (tool == m_tools.end()) {
    recordToolResult(id, ToolState::Failed, {.content = tr("This tool is not enabled.").toStdString()});
    if (running() && generation == m_generation) schedule([this] { executeNextTool(); });
    return;
  }

  m_toolTimer.start();
  call.state = ToolState::Running;
  setActivity(ResponseActivity::RunningTool);
  if (!running() || generation != m_generation) return;
  emit toolChanged(id);
  if (!running() || generation != m_generation) return;

  AbstractTool::RawToolTask task;

  try {
    task = (*tool)->runRaw(call.call.arguments);
  } catch (const std::exception &error) {
    if (!running() || generation != m_generation) return;

    recordToolResult(id, ToolState::Failed, {.content = error.what()});
    if (running() && generation == m_generation) schedule([this] { executeNextTool(); });
    return;
  }

  if (!running() || generation != m_generation) {
    if (task.cancel) task.cancel();
    return;
  }

  m_cancelTool = std::move(task.cancel);
  auto *watcher = new QFutureWatcher<AbstractTool::RawToolResult>(this);
  m_toolWatcher = watcher;

  connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher, id, generation] {
    if (!running() || generation != m_generation) return;

    m_toolWatcher = nullptr;
    m_cancelTool = {};
    watcher->deleteLater();

    AbstractTool::RawToolResult result =
        std::unexpected(tr("The tool did not return a result.").toStdString());
    try {
      if (!watcher->isCanceled() && watcher->future().resultCount() > 0) result = watcher->result();
    } catch (const std::exception &error) { result = std::unexpected(error.what()); }

    const auto state = result && !result->failed ? ToolState::Succeeded : ToolState::Failed;
    recordToolResult(id, state,
                     result ? std::move(*result) : ToolOutput{.content = std::move(result.error())});
    if (running() && generation == m_generation) schedule([this] { executeNextTool(); });
  });

  watcher->setFuture(task.future);
}

void Agent::recordToolResult(Id id, ToolState state, ToolOutput output) {
  auto &call = *std::ranges::find(m_calls, id, &ToolCall::id);
  if (call.state == ToolState::Running) call.duration = std::chrono::milliseconds(m_toolTimer.elapsed());
  call.state = state;
  output.failed = state == ToolState::Failed;
  call.output = std::move(output);

  appendMessage(
      ChatMessage{.role = ChatRole::Tool, .parts = {ToolResultPart{call.call.id, call.output->content}}});
  emit toolChanged(id);
}

void Agent::stopWork() {
  ++m_generation;

  if (auto completion = std::exchange(m_completion, {})) {
    disconnect(completion.get(), nullptr, this, nullptr);
    completion->abort();
  }

  if (auto *watcher = std::exchange(m_toolWatcher, nullptr)) {
    disconnect(watcher, nullptr, this, nullptr);
    watcher->deleteLater();
  }

  if (auto cancel = std::exchange(m_cancelTool, {})) cancel();
}

void Agent::finish(State state, std::optional<std::string> error) {
  if (!running() || m_finishing) return;

  m_finishing = true;
  stopWork();

  for (const auto id : m_pendingCalls) {
    if (!toolCall(id)->output)
      recordToolResult(id, ToolState::Cancelled,
                       {.content = tr("Tool execution was cancelled.").toStdString()});
  }

  m_pendingCalls.clear();
  m_state = state;
  m_error = std::move(error);

  emit stateChanged();
  m_finishing = false;
  emit finished();
}

void Agent::cancel() { finish(State::Cancelled); }

void Agent::setActivity(ResponseActivity activity) {
  if (m_activity == activity) return;
  m_activity = activity;
  emit activityChanged();
}

} // namespace AI
