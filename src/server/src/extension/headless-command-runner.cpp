#include "extension/headless-command-runner.hpp"
#include "common/context.hpp"
#include "extension/extension-command.hpp"
#include "extension/manager/extension-manager.hpp"
#include "service-registry.hpp"

std::expected<QPointer<ExtensionCommandRuntime>, std::string>
HeadlessCommandRunner::run(const std::shared_ptr<ExtensionCommand> &command,
                           ExtensionCommandRuntime::RunCallback onFinished,
                           std::optional<ExtensionCommandRuntime::ToolCall> toolCall) {
  if (!m_ctx.services->extensionManager()->isRunning()) {
    return std::unexpected(tr("The extension runtime is not running.").toStdString());
  }

  auto *runtime = new ExtensionCommandRuntime(command);
  QPointer handle(runtime);

  runtime->setContext(&m_ctx);
  runtime->setHeadless(std::move(onFinished));
  if (toolCall) runtime->setToolCall(std::move(*toolCall));
  runtime->load({});

  return handle;
}
