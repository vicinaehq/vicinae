#include "extension-manifest.hpp"
#include <charconv>
#include <glaze/json/read.hpp>
#include <glaze/json/generic.hpp>
#include <glaze/json/minify.hpp>
#include <glaze/json/raw_string.hpp>
#include <map>
#include "utils.hpp"
#include "vicinae.hpp"

namespace fs = std::filesystem;

// glaze reflection needs types with linkage, so these can't live in an anonymous namespace
namespace manifest_json {

constexpr auto MANIFEST_READ_OPTS = glz::opts{.error_on_unknown_keys = false};
constexpr std::string_view AI_MENTION_SEARCH = "AI mention search";

struct RawDropdownItem {
  std::string title;
  std::string value;
};

struct RawPreference {
  std::string name;
  std::string type;
  std::string title;
  std::string description;
  std::string placeholder;
  std::string label;
  bool required = false;
  bool multiple = false;
  std::optional<glz::generic> defaultValue;
  std::vector<RawDropdownItem> data;
};

struct RawArgument {
  std::string name;
  std::string type;
  std::string placeholder;
  bool required = false;
  std::vector<RawDropdownItem> data;
};

struct RawCommand {
  std::string name;
  std::string title;
  std::string description;
  std::string mode;
  std::optional<std::string> icon;
  std::optional<std::string> interval;
  bool disabledByDefault = false;
  std::vector<std::string> keywords;
  std::vector<RawPreference> preferences;
  std::vector<RawArgument> arguments;
};

struct RawTool {
  std::string name;
  std::string title;
  std::string description;
  std::optional<std::string> instructions;
  std::optional<std::string> icon;
  std::optional<glz::raw_json> input;
  bool confirmation = false;
  std::vector<std::string> functionalities;
};

struct RawManifest {
  std::string name;
  std::string title;
  std::string description;
  std::string icon;
  std::string author;
  std::vector<std::string> categories;
  std::vector<RawPreference> preferences;
  std::vector<RawCommand> commands;
  std::vector<RawTool> tools;
  std::map<std::string, std::string> dependencies;
};

} // namespace manifest_json

// NOLINTBEGIN(readability-identifier-naming)
template <> struct glz::meta<manifest_json::RawPreference> {
  static constexpr std::string_view rename_key(std::string_view key) {
    return key == "defaultValue" ? "default" : key;
  }
};
// NOLINTEND(readability-identifier-naming)

namespace {

using namespace manifest_json;

QString qstr(const std::string &str) { return QString::fromStdString(str); }

std::expected<std::chrono::seconds, QString> parseInterval(std::string_view str) {
  if (str.empty()) return std::unexpected(QString("interval is empty"));

  char const unit = str.back();

  std::int64_t value = 0;
  auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size() - 1, value);
  if (ec != std::errc{} || ptr != str.data() + str.size() - 1)
    return std::unexpected(QString("invalid interval format: %1").arg(qstr(std::string{str})));

  using namespace std::chrono;

  seconds secs;

  switch (unit) {
  case 's':
    secs = seconds(value);
    break;
  case 'm':
    secs = duration_cast<seconds>(minutes(value));
    break;
  case 'h':
    secs = duration_cast<seconds>(hours(value));
    break;
  case 'd':
    secs = duration_cast<seconds>(hours(value * 24));
    break;
  default:
    return std::unexpected(QString("unknown interval unit: %1").arg(unit));
  }

  if (secs < seconds(5))
    return std::unexpected(QString("interval must be at least 5s, got %1s").arg(secs.count()));

  return secs;
}

Preference parsePreference(RawPreference &raw) {
  Preference base;

  base.setTitle(qstr(raw.title));
  base.setDescription(qstr(raw.description));
  base.setName(qstr(raw.name));
  base.setPlaceholder(qstr(raw.placeholder));
  base.setRequired(raw.required);
  if (raw.defaultValue) base.setDefaultValue(std::move(*raw.defaultValue));

  const auto &type = raw.type;

  if (type == "textfield") {
    base.setData(Preference::TextData());
  } else if (type == "password") {
    base.setData(Preference::PasswordData());
  } else if (type == "checkbox") {
    base.setData(Preference::CheckboxData{.label = qstr(raw.label)});
  } else if (type == "appPicker") {
    base.setData(Preference::AppPickerData{.multiple = raw.multiple});
  } else if (type == "file") {
    base.setData(Preference::FilePickerData{.multiple = raw.multiple});
  } else if (type == "directory") {
    base.setData(Preference::DirectoryPickerData{.multiple = raw.multiple});
  } else if (type == "dropdown") {
    std::vector<Preference::DropdownData::Option> options;
    options.reserve(raw.data.size());

    for (const auto &item : raw.data) {
      options.emplace_back(
          Preference::DropdownData::Option{.title = qstr(item.title), .value = qstr(item.value)});
    }

    base.setData(Preference::DropdownData{options});
  } else {
    qWarning() << "Unknown extension preference type" << qstr(type);
  }

  return base;
}

CommandArgument parseArgument(const RawArgument &raw) {
  CommandArgument arg{
      .name = qstr(raw.name),
      .type = CommandArgument::Text,
      .placeholder = qstr(raw.placeholder),
      .required = raw.required,
  };

  if (raw.type == "password") arg.type = CommandArgument::Password;

  if (raw.type == "dropdown") {
    std::vector<CommandArgument::DropdownData> options;
    options.reserve(raw.data.size());

    for (const auto &item : raw.data) {
      options.emplace_back(
          CommandArgument::DropdownData{.title = qstr(item.title), .value = qstr(item.value)});
    }

    arg.type = CommandArgument::Dropdown;
    arg.data = std::move(options);
  }

  return arg;
}

ExtensionManifest::Command parseCommand(RawCommand &raw) {
  ExtensionManifest::Command command;

  command.name = qstr(raw.name);
  command.title = qstr(raw.title);
  command.description = qstr(raw.description);
  command.defaultDisabled = raw.disabledByDefault;
  if (raw.icon) command.icon = qstr(*raw.icon);

  if (raw.mode == "view") {
    command.mode = CommandMode::CommandModeView;
  } else if (raw.mode == "no-view") {
    command.mode = CommandMode::CommandModeNoView;
  } else {
    command.mode = CommandMode::CommandModeInvalid;
  }

  if (raw.interval && command.mode == CommandModeNoView) {
    if (auto interval = parseInterval(*raw.interval)) {
      command.interval = *interval;
    } else {
      qWarning() << "Failed to parse interval for command" << command.name << "-" << interval.error();
    }
  }

  command.keywords.reserve(raw.keywords.size());
  for (const auto &keyword : raw.keywords) {
    command.keywords.emplace_back(qstr(keyword));
  }

  command.preferences.reserve(raw.preferences.size());
  for (auto &pref : raw.preferences) {
    command.preferences.emplace_back(parsePreference(pref));
  }

  command.arguments.reserve(raw.arguments.size());
  for (const auto &arg : raw.arguments) {
    command.arguments.emplace_back(parseArgument(arg));
  }

  return command;
}

ExtensionManifest::Tool parseTool(RawTool &raw, const fs::path &root) {
  return ExtensionManifest::Tool{
      .name = raw.name,
      .title = qstr(raw.title),
      .description = qstr(raw.description),
      .instructions = std::move(raw.instructions),
      .icon = raw.icon.transform(qstr),
      .inputSchema = raw.input.transform([](glz::raw_json &json) { return glz::minify_json(json.str); }),
      .confirmation = raw.confirmation,
      .entrypoint = root / "tools" / std::format("{}.js", raw.name),
  };
}

} // namespace

std::expected<ExtensionManifest, ManifestError> ExtensionManifest::fromPackageJson(const fs::path &path) {
  fs::path const manifestPath = path / "package.json";
  RawManifest raw;
  std::string buf;

  if (const auto error = glz::read_file_json<MANIFEST_READ_OPTS>(raw, manifestPath.string(), buf)) {
    return std::unexpected<ManifestError>(QString("Failed to parse %1: %2")
                                              .arg(qstr(manifestPath.string()))
                                              .arg(qstr(glz::format_error(error, buf))));
  }

  ExtensionManifest manifest;

  manifest.path = path;
  manifest.id = qstr(getLastPathComponent(path));
  manifest.name = qstr(raw.name);
  manifest.title = qstr(raw.title);
  manifest.description = qstr(raw.description);
  manifest.icon = qstr(raw.icon);
  manifest.author = qstr(raw.author);
  manifest.needsRaycastApi = raw.dependencies.contains(Omnicast::RAYCAST_NPM_API_PACKAGE.toStdString());

  if (manifest.id.startsWith("store.vicinae.")) {
    manifest.provenance = ExtensionManifest::Provenance::Vicinae;
  } else if (manifest.id.startsWith("store.raycast.")) {
    manifest.provenance = ExtensionManifest::Provenance::Raycast;
  } else {
    manifest.provenance = ExtensionManifest::Provenance::Local;
  }

  manifest.categories.reserve(raw.categories.size());
  for (const auto &category : raw.categories) {
    manifest.categories.emplace_back(qstr(category));
  }

  manifest.commands.reserve(raw.commands.size());
  for (auto &rawCommand : raw.commands) {
    auto command = parseCommand(rawCommand);

    if (command.mode != CommandModeView && command.mode != CommandModeNoView) continue;

    command.provenance = manifest.provenance;
    command.entrypoint = path / std::format("{}.js", rawCommand.name);
    manifest.commands.emplace_back(std::move(command));
  }

  manifest.preferences.reserve(raw.preferences.size());
  for (auto &pref : raw.preferences) {
    manifest.preferences.emplace_back(parsePreference(pref));
  }

  manifest.tools.reserve(raw.tools.size());
  for (auto &tool : raw.tools) {
    if (std::ranges::contains(tool.functionalities, AI_MENTION_SEARCH)) continue;
    manifest.tools.emplace_back(parseTool(tool, path));
  }

  return manifest;
}
