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

class HeadlessCommandRunner {
  Q_DECLARE_TR_FUNCTIONS(HeadlessCommandRunner)

public:
  explicit HeadlessCommandRunner(ApplicationContext &ctx) : m_ctx(ctx) {}

  // `onFinished` is called once, possibly before this returns, unless the command could not be started
  std::expected<QPointer<ExtensionCommandRuntime>, std::string>
  run(const std::shared_ptr<ExtensionCommand> &command, ExtensionCommandRuntime::RunCallback onFinished,
      std::optional<ExtensionCommandRuntime::ToolCall> toolCall = std::nullopt);

private:
  ApplicationContext &m_ctx;
};
