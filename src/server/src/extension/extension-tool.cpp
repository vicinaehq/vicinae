#include "extension/extension-tool.hpp"
#include <QPointer>
#include <QPromise>
#include <format>
#include <glaze/json/generic.hpp>
#include <glaze/json/read.hpp>
#include "common/context.hpp"
#include "extension/extension-command-runtime.hpp"
#include "extension/extension-command.hpp"
#include "extension/manager/extension-manager.hpp"
#include "service-registry.hpp"
#include "services/ai/tool-registry.hpp"

namespace {

constexpr std::string_view EMPTY_INPUT_SCHEMA = R"({"type":"object","properties":{}})";

AI::AbstractTool::RawToolTask ready(AI::AbstractTool::RawToolResult result) {
  return {QtFuture::makeReadyValueFuture(std::move(result)), {}};
}

} // namespace

AI::AbstractTool::RawToolTask ExtensionToolRunner::run(const std::shared_ptr<ExtensionCommand> &command,
                                                       std::string_view arguments, bool requiresApproval) {
  glz::generic input;
  if (const auto error = glz::read_json(input, arguments)) {
    return ready(std::unexpected(glz::format_error(error, arguments)));
  }

  if (!m_ctx.services->extensionManager()->isRunning()) {
    return ready(std::unexpected(tr("The extension runtime is not running.").toStdString()));
  }

  const auto promise = std::make_shared<QPromise<AI::AbstractTool::RawToolResult>>();
  auto *runtime = new ExtensionCommandRuntime(command);

  promise->start();
  runtime->setContext(&m_ctx);
  runtime->setHeadless(true);
  runtime->setToolInvocation({
      .input = std::move(input),
      .requiresApproval = requiresApproval,
      .done =
          [promise, runtime](ExtensionCommandRuntime::ToolResult result) {
            promise->addResult(result.transform(
                [](std::string output) { return AI::ToolOutput{.content = std::move(output)}; }));
            promise->finish();
            runtime->unload();
            runtime->deleteLater();
          },
  });

  runtime->load({});

  return {
      promise->future(),
      [runtime = QPointer(runtime)] {
        if (runtime) runtime->cancelTool(tr("The tool was cancelled.").toStdString());
      },
  };
}

std::string ExtensionTool::description() const {
  auto description = m_tool.description.toStdString();
  if (!m_tool.instructions) return description;
  return std::format("{}\n\n{}", description, *m_tool.instructions);
}

std::string ExtensionTool::generateInputSchema() const {
  return m_tool.inputSchema.value_or(std::string{EMPTY_INPUT_SCHEMA});
}

AI::AbstractTool::RawToolTask ExtensionTool::runRaw(std::string_view arguments) {
  auto *runner = ServiceRegistry::instance()->extensionToolRunner();
  if (!runner) return ready(std::unexpected(tr("Extension tools are not available.").toStdString()));
  const auto id = m_command->uniqueId();
  const auto policy = ServiceRegistry::instance()->tools()->toolPolicy(id.provider, m_tool.name);
  return runner->run(m_command, arguments, policy != AI::ToolPolicy::Allow);
}
