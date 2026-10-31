#include <filesystem>
#include <format>
#include <glaze/json/patch.hpp>
#include <QFile>
#include "config-tools.hpp"
#include "config/config.hpp"

namespace {

struct WriteConfigInput {
  glz::raw_json patch;
};

AI::AbstractTool::RawToolTask ready(AI::AbstractTool::RawToolResult result) {
  return {QtFuture::makeReadyValueFuture(std::move(result)), {}};
}

} // namespace

std::string ReadConfigTool::description() const {
  return "Read the Vicinae configuration. Returns the default configuration, whose comments document every "
         "available key, followed by the user configuration file, which only contains the values the user "
         "overrides. Read it before changing the configuration with write_config.";
}

std::string ReadConfigTool::generateInputSchema() const {
  return R"({"type":"object","properties":{},"additionalProperties":false})";
}

AI::AbstractTool::RawToolTask ReadConfigTool::runRaw(std::string_view) {
  const auto path = m_config.path();
  std::string user;

  if (QFile file(path); file.open(QIODevice::ReadOnly)) {
    user = file.readAll().toStdString();
  } else {
    user = "(the user configuration file does not exist yet)";
  }

  return ready(AI::ToolOutput{
      .content = std::format("Default configuration:\n\n{}\n\nUser configuration file ({}):\n\n{}",
                             m_config.defaultConfigData(), path.string(), user),
  });
}

std::string WriteConfigTool::description() const {
  return "Change the Vicinae user configuration. Always call read_config first. The patch argument is a JSON "
         "merge patch (RFC 7386) applied to the user configuration file: only include the keys to change, "
         "nested objects are merged, arrays replace the existing value, and null removes a key so that it "
         "goes back to its default value. Keys must exist in the default configuration. Changes apply "
         "immediately. Secrets such as API keys are not stored in this file and cannot be set with this "
         "tool.";
}

std::string WriteConfigTool::generateInputSchema() const {
  return R"({"type":"object","properties":{"patch":{"type":"object","description":"JSON merge patch applied to the user configuration file."}},"required":["patch"],"additionalProperties":false})";
}

std::optional<std::string> WriteConfigTool::invocationSummary(std::string_view arguments) const {
  WriteConfigInput input;
  if (glz::read_json(input, arguments)) return std::nullopt;
  return glz::prettify_json(input.patch.str);
}

AI::AbstractTool::RawToolTask WriteConfigTool::runRaw(std::string_view arguments) {
  WriteConfigInput input;
  if (const auto error = glz::read_json(input, arguments))
    return ready(std::unexpected(glz::format_error(error, arguments)));

  const auto path = m_config.path();
  config::Partial<config::ConfigValue> current;
  std::string buffer;

  if (std::filesystem::exists(path)) {
    if (const auto error =
            glz::read_file_jsonc<glz::opts{.error_on_unknown_keys = false}>(current, path.string(), buffer))
      return ready(std::unexpected(std::format("The user configuration file {} is invalid: {}", path.string(),
                                               glz::format_error(error))));
  }

  glz::generic document;
  if (const auto error = glz::write_json(current, buffer); error || glz::read_json(document, buffer))
    return ready(std::unexpected(std::string("Could not read the current configuration.")));

  if (const auto error = glz::merge_patch(document, input.patch.str))
    return ready(
        std::unexpected(std::format("Invalid patch: {}", glz::format_error(error, input.patch.str))));

  if (const auto error = glz::write_json(document, buffer))
    return ready(std::unexpected(std::string("Could not apply the patch.")));

  config::Partial<config::ConfigValue> value;
  if (const auto error = glz::read_json(value, buffer))
    return ready(std::unexpected(
        std::format("The patched configuration is invalid: {}", glz::format_error(error, buffer))));

  if (!m_config.replaceUser(value))
    return ready(std::unexpected(std::format("Could not write {}.", path.string())));

  return ready(AI::ToolOutput{.content = "The configuration was updated."});
}
