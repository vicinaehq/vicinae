#pragma once
#include "common/context.hpp"
#include "ai-tool.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/ai/ai-service.hpp"
#include <format>
#include <qfuturewatcher.h>
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <vector>

namespace AI {
class Agent : public QObject {
  Q_OBJECT

signals:
  void toolStarted(const std::string &name, const std::string &description) const;
  void toolEnded(std::string_view output) const;
  void tokenAdded(std::string_view name);

public:
  Agent(const ApplicationContext &ctx) : m_ctx(ctx) {
    addTool(std::make_unique<GenerateFunFact>());
    connect(&m_currentToolWatcher, &decltype(m_currentToolWatcher)::finished, this, &Agent::handleToolResult);
  }

  void addTool(std::unique_ptr<AbstractTool> tool) { m_tools.emplace_back(std::move(tool)); }

  void addMessage(std::string_view content) {
    m_messages.emplace_back(AI::ChatMessage::fromText(AI::ChatRole::User, std::string{content}));
    triggerCompletion();
  }

private:
  void triggerCompletion() {
    auto tools = m_tools | std::views::transform([](auto &&tool) { return tool.get(); }) |
                 std::ranges::to<std::vector>();
    m_completion = m_ctx.services->ai()->createChatCompletion({}, {.messages = m_messages, .tools = tools});

    connect(m_completion.get(), &AbstractChatCompletionStream::dataAdded, this, &Agent::handleDataAdded);
    connect(m_completion.get(), &AbstractChatCompletionStream::toolCallRequested, this,
            &Agent::handleToolCall);

    m_completion->start();
  }

  void handleDataAdded(std::string_view token) { emit tokenAdded(token); }

  void handleToolResult() {
    auto result = m_currentToolWatcher.result();
    auto output = result ? std::move(*result) : std::move(result.error());

    emit toolEnded(output);
    qDebug() << "tool ended with result" << output;
    addToolResult(std::move(output));
  }

  void handleToolCall(const ToolCallPart &call) {
    qDebug() << "Requesting tool call" << call.name << "with call ID" << call.id;
    m_pendingCall = call;
    m_messages.emplace_back(AI::ChatMessage{.role = AI::ChatRole::Assistant, .parts = {call}});

    for (const auto &tool : m_tools) {
      if (call.name == tool->name()) {
        qDebug() << "Found tool" << tool->name() << tool->description();
        m_currentToolWatcher.setFuture(tool->runRaw(call.arguments));
        emit toolStarted(call.name, tool->description());
        return;
      }
    }

    addToolResult(std::format("No tool with name {} is available", call.name));
  }

  void addToolResult(std::string content) {
    m_messages.emplace_back(AI::ChatMessage{
        .role = AI::ChatRole::Tool,
        .parts = {ToolResultPart{.callId = m_pendingCall.id, .content = std::move(content)}},
    });
    triggerCompletion();
  }

  std::shared_ptr<AbstractChatCompletionStream> m_completion;
  std::vector<std::unique_ptr<AbstractTool>> m_tools;
  QFutureWatcher<std::expected<std::string, std::string>> m_currentToolWatcher;
  ToolCallPart m_pendingCall;
  const ApplicationContext &m_ctx;
  AI::ChatHistory m_messages;
};
}; // namespace AI
