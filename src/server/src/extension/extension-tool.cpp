#include "extension/extension-tool.hpp"
#include <QPointer>
#include <QPromise>
#include <format>
#include <glaze/json/generic.hpp>
#include <glaze/json/read.hpp>
#include "extension/extension-command-runtime.hpp"
#include "extension/extension-command.hpp"
#include "extension/headless-command-runner.hpp"
#include "service-registry.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/settings/settings-controller.hpp"

namespace {

constexpr std::string_view EMPTY_INPUT_SCHEMA = R"({"type":"object","properties":{}})";

AI::AbstractTool::RawToolTask ready(AI::AbstractTool::RawToolResult result) {
  return {QtFuture::makeReadyValueFuture(std::move(result)), {}};
}

} // namespace

PreferenceValues ExtensionToolCommand::preferenceValues(const RootItemManager &rootItemManager) const {
  const auto id = uniqueId();
  auto values = rootItemManager.getProviderPreferenceValues(QString::fromStdString(id.provider));
  for (auto &[key, value] : rootItemManager.getToolPreferenceValues(id, preferences())) {
    values.insert_or_assign(key, std::move(value));
  }
  return values;
}

void ExtensionToolCommand::openPreferences(SettingsController &settings) const {
  settings.openToolPreferences(uniqueId());
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
  auto *runner = ServiceRegistry::instance()->headlessCommandRunner();
  if (!runner) return ready(std::unexpected(tr("Extension tools are not available.").toStdString()));

  glz::generic input;
  if (const auto error = glz::read_json(input, arguments)) {
    return ready(std::unexpected(glz::format_error(error, arguments)));
  }

  const auto promise = std::make_shared<QPromise<RawToolResult>>();
  promise->start();

  const auto runtime = runner->run(
      m_command,
      [promise](ExtensionCommandRuntime::RunResult result) {
        promise->addResult(result.transform(
            [](std::string output) { return AI::ToolOutput{.content = std::move(output)}; }));
        promise->finish();
      },
      ExtensionCommandRuntime::ToolCall{.input = std::move(input)});
  if (!runtime) return ready(std::unexpected(runtime.error()));

  return {
      promise->future(),
      [runtime = *runtime] {
        if (runtime) runtime->cancel(tr("The tool was cancelled.").toStdString());
      },
  };
}
