#pragma once
#include <QCoreApplication>
#include <QPointer>
#include <expected>
#include <memory>
#include <optional>
#include <string>
#include "extension/extension-command-runtime.hpp"

struct ApplicationContext;
class ExtensionCommand;

/**
 * Runs extension commands without UI: interval commands and AI tools.
 */
class HeadlessCommandRunner {
  Q_DECLARE_TR_FUNCTIONS(HeadlessCommandRunner)

public:
  explicit HeadlessCommandRunner(ApplicationContext &ctx) : m_ctx(ctx) {}

  /**
   * Starts the command, as a tool when `toolCall` is set. `onFinished` is called exactly once with the
   * outcome, possibly before this returns. Returns the running command, to cancel it, or why it could not
   * be started, in which case `onFinished` is never called.
   */
  std::expected<QPointer<ExtensionCommandRuntime>, std::string>
  run(const std::shared_ptr<ExtensionCommand> &command, ExtensionCommandRuntime::RunCallback onFinished,
      std::optional<ExtensionCommandRuntime::ToolCall> toolCall = std::nullopt);

private:
  ApplicationContext &m_ctx;
};
