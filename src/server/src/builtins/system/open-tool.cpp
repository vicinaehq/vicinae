#include <filesystem>
#include <format>
#include <QDir>
#include "open-tool.hpp"
#include "services/app-service/app-service.hpp"

namespace {

struct OpenInput {
  std::string target;
};

AI::AbstractTool::RawToolTask ready(AI::AbstractTool::RawToolResult result) {
  return {QtFuture::makeReadyValueFuture(std::move(result)), {}};
}

QString expandHome(const std::string &target) {
  if (target == "~") return QDir::homePath();
  if (target.starts_with("~/")) return QDir::home().filePath(QString::fromStdString(target.substr(2)));
  return QString::fromStdString(target);
}

} // namespace

std::string OpenTool::description() const {
  return "Open a file, directory or URL on the user's computer with its default application, as if the user "
         "opened it themselves. Files and directories need an absolute path.";
}

std::string OpenTool::generateInputSchema() const {
  return R"({"type":"object","properties":{"target":{"type":"string","description":"Absolute path or URL to open. ~ is expanded to the home directory."}},"required":["target"],"additionalProperties":false})";
}

std::optional<std::string> OpenTool::invocationSummary(std::string_view arguments) const {
  OpenInput input;
  if (glz::read_json(input, arguments)) return std::nullopt;
  return input.target;
}

AI::AbstractTool::RawToolTask OpenTool::runRaw(std::string_view arguments) {
  OpenInput input;
  if (const auto error = glz::read_json(input, arguments))
    return ready(std::unexpected(glz::format_error(error, arguments)));

  const auto target = expandHome(input.target);
  std::error_code ec;
  if (QDir::isAbsolutePath(target) && !std::filesystem::exists(target.toStdString(), ec))
    return ready(std::unexpected(std::format("{} does not exist.", target.toStdString())));

  const auto app = m_apps.findDefaultOpener(target);
  if (!app) return ready(std::unexpected(std::format("No application can open {}.", target.toStdString())));
  if (!m_apps.launch(*app, {target}))
    return ready(std::unexpected(std::format("{} could not be started.", app->displayName().toStdString())));

  return ready(AI::ToolOutput{
      .content = std::format("Opened {} with {}.", target.toStdString(), app->displayName().toStdString())});
}
