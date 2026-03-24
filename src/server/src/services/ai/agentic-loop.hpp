#pragma once
#include "common/context.hpp"
#include "ai-tool.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/ai/ai-service.hpp"
#include <absl/strings/str_format.h>
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
    m_messages.emplace_back(AI::ChatMessage(AI::ChatRole::User, std::string{content}));
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

    emit toolEnded(result.value());
    qDebug() << "tool ended with result" << result.value();
    m_messages.emplace_back(AI::ChatMessage(AI::ChatRole::Tool, result.value()));
    triggerCompletion();
  }

  void handleToolCall(const AbstractChatCompletionStream::ToolCallRequest &call) {
    qDebug() << "Requesting tool call" << call.function.name << "with call ID" << call.id;
    for (const auto &tool : m_tools) {
      if (call.function.name == tool->name()) {
        qDebug() << "Found tool" << tool->name() << tool->description();
        m_currentToolWatcher.setFuture(tool->runRaw(call.function.arguments.str));
        emit toolStarted(call.function.name, tool->description());
        return;
      }
    }

    m_messages.emplace_back(AI::ChatMessage(
        AI::ChatRole::System, std::format("No tool with name {} is available", call.function.name)));
    triggerCompletion();

    // no such tool
  }

  std::shared_ptr<AbstractChatCompletionStream> m_completion;
  std::vector<std::unique_ptr<AbstractTool>> m_tools;
  QFutureWatcher<std::expected<std::string, std::string>> m_currentToolWatcher;
  const ApplicationContext &m_ctx;
  AI::ChatHistory m_messages;
};
}; // namespace AI
