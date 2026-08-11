#include "extension-manifest.hpp"
#include "glaze-qt.hpp"
#include "utils.hpp"
#include "vicinae.hpp"
#include <QDebug>
#include <charconv>
#include <format>
#include <glaze/glaze.hpp>
#include <map>
#include <ranges>
#include <set>

namespace fs = std::filesystem;

namespace manifest_dto {

struct DropdownOption {
  QString title;
  QString value;
};

struct Preference {
  QString type;
  QString name;
  QString title;
  QString description;
  QString placeholder;
  std::optional<QString> label;
  bool required = false;
  bool multiple = false;
  std::vector<DropdownOption> data;
  std::optional<glz::generic> defaultValue;
};

struct Argument {
  QString type;
  QString name;
  QString placeholder;
  bool required = false;
  std::vector<DropdownOption> data;
};

struct Command {
  QString name;
  QString title;
  QString description;
  QString mode;
  bool disabledByDefault = false;
  std::optional<QString> icon;
  std::optional<QString> interval;
  std::vector<QString> keywords;
  std::vector<Preference> preferences;
  std::vector<Argument> arguments;
};

struct Package {
  QString name;
  QString title;
  QString description;
  QString icon;
  QString author;
  std::vector<QString> categories;
  std::vector<Command> commands;
  std::vector<Preference> preferences;
  std::map<std::string, glz::raw_json> dependencies;
};

} // namespace manifest_dto

template <> struct glz::meta<manifest_dto::Preference> {
  static constexpr std::string_view rename_key(const std::string_view key) {
    return key == "defaultValue" ? "default" : key;
  }
};

static Preference parsePreference(const manifest_dto::Preference &dto) {
  Preference base;

  base.setTitle(dto.title);
  base.setDescription(dto.description);
  base.setName(dto.name);
  base.setPlaceholder(dto.placeholder);
  base.setRequired(dto.required);
  base.setDefaultValue(dto.defaultValue ? glazeToQJsonValue(*dto.defaultValue)
                                        : QJsonValue(QJsonValue::Undefined));

  if (dto.type == "textfield") {
    base.setData(Preference::TextData());
  } else if (dto.type == "password") {
    base.setData(Preference::PasswordData());
  } else if (dto.type == "checkbox") {
    base.setData(Preference::CheckboxData{.label = dto.label});
  } else if (dto.type == "appPicker") {
    base.setData(Preference::AppPickerData());
  } else if (dto.type == "file") {
    base.setData(Preference::FilePickerData{.multiple = dto.multiple});
  } else if (dto.type == "directory") {
    base.setData(Preference::DirectoryPickerData{.multiple = dto.multiple});
  } else if (dto.type == "dropdown") {
    auto options = dto.data | std::views::transform([](const manifest_dto::DropdownOption &opt) {
                     return Preference::DropdownData::Option{.title = opt.title, .value = opt.value};
                   }) |
                   std::ranges::to<std::vector>();

    base.setData(Preference::DropdownData{std::move(options)});
  } else {
    qWarning() << "Unknown extension preference type" << dto.type;
  }

  return base;
}

static CommandArgument parseArgument(const manifest_dto::Argument &dto) {
  CommandArgument arg;

  if (dto.type == "text") arg.type = CommandArgument::Text;
  if (dto.type == "password") arg.type = CommandArgument::Password;
  if (dto.type == "dropdown") arg.type = CommandArgument::Dropdown;

  arg.name = dto.name;
  arg.placeholder = dto.placeholder;
  arg.required = dto.required;

  if (dto.type == "dropdown") {
    arg.data = dto.data | std::views::transform([](const manifest_dto::DropdownOption &opt) {
                 return CommandArgument::DropdownData{.title = opt.title, .value = opt.value};
               }) |
               std::ranges::to<std::vector>();
  }

  return arg;
}

static std::expected<std::chrono::seconds, QString> parseInterval(const std::string &str) {
  if (str.empty()) return std::unexpected(QString("interval is empty"));

  char unit = str.back();

  std::int64_t value = 0;
  auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size() - 1, value);
  if (ec != std::errc{} || ptr != str.data() + str.size() - 1)
    return std::unexpected(QString("invalid interval format: %1").arg(str.c_str()));

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

static ExtensionManifest::Command parseCommand(const manifest_dto::Command &dto) {
  ExtensionManifest::Command command;

  command.name = dto.name;
  command.title = dto.title;
  command.description = dto.description;
  command.defaultDisabled = dto.disabledByDefault;
  command.icon = dto.icon;

  if (dto.mode == "view") {
    command.mode = CommandMode::CommandModeView;
  } else if (dto.mode == "no-view") {
    command.mode = CommandMode::CommandModeNoView;
  } else {
    command.mode = CommandMode::CommandModeInvalid;
  }

  if (dto.interval && command.mode == CommandModeNoView) {
    auto interval = parseInterval(dto.interval->toStdString());
    if (interval) {
      command.interval = *interval;
    } else {
      qWarning() << "Failed to parse interval for command" << command.name << "-" << interval.error();
    }
  }

  command.keywords = dto.keywords;
  command.preferences =
      dto.preferences | std::views::transform(parsePreference) | std::ranges::to<std::vector>();
  command.arguments = dto.arguments | std::views::transform(parseArgument) | std::ranges::to<std::vector>();

  return command;
}

std::expected<ExtensionManifest, ManifestError> ExtensionManifest::fromPackageJson(const fs::path &path) {
  static const std::set<CommandMode> supportedModes{CommandMode::CommandModeView, CommandModeNoView};
  fs::path const manifestPath = path / "package.json";
  std::error_code ec{};

  if (!fs::exists(manifestPath, ec)) {
    return std::unexpected<ManifestError>(
        QString("Could not find package.json file at %1").arg(manifestPath.c_str()));
  }

  manifest_dto::Package pkg;
  std::string buf;

  if (auto error =
          glz::read_file_json<glz::opts{.error_on_unknown_keys = false}>(pkg, manifestPath.string(), buf)) {
    return std::unexpected<ManifestError>(QString("Failed to parse package.json at %1: %2")
                                              .arg(manifestPath.c_str())
                                              .arg(QString::fromStdString(glz::format_error(error, buf))));
  }

  ExtensionManifest manifest;

  manifest.path = path;
  manifest.id = QString::fromStdString(getLastPathComponent(path));
  manifest.name = pkg.name;
  manifest.title = pkg.title;
  manifest.description = pkg.description;
  manifest.icon = pkg.icon;
  manifest.author = pkg.author;
  manifest.needsRaycastApi = pkg.dependencies.contains(Omnicast::RAYCAST_NPM_API_PACKAGE.toStdString());

  if (manifest.id.startsWith("store.vicinae.")) {
    manifest.provenance = ExtensionManifest::Provenance::Vicinae;
  } else if (manifest.id.startsWith("store.raycast.")) {
    manifest.provenance = ExtensionManifest::Provenance::Raycast;
  } else {
    manifest.provenance = ExtensionManifest::Provenance::Local;
  }

  manifest.categories = std::move(pkg.categories);
  manifest.preferences =
      pkg.preferences | std::views::transform(parsePreference) | std::ranges::to<std::vector>();

  manifest.commands.reserve(pkg.commands.size());

  for (const auto &dto : pkg.commands) {
    auto command = parseCommand(dto);

    command.provenance = manifest.provenance;
    command.entrypoint = path / std::format("{}.js", command.name.toStdString());

    if (supportedModes.contains(command.mode)) { manifest.commands.emplace_back(std::move(command)); }
  }

  return manifest;
}
