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
  using T = manifest_dto::Preference;
  static constexpr auto value =
      object(&T::type, &T::name, &T::title, &T::description, &T::placeholder, &T::label, &T::required,
             &T::multiple, &T::data, "default", &T::defaultValue);
};

static Preference parsePreference(manifest_dto::Preference &&dto) {
  Preference base;

  base.setTitle(std::move(dto.title));
  base.setDescription(std::move(dto.description));
  base.setName(std::move(dto.name));
  base.setPlaceholder(std::move(dto.placeholder));
  base.setRequired(dto.required);
  base.setDefaultValue(dto.defaultValue ? glazeToQJsonValue(*dto.defaultValue)
                                        : QJsonValue(QJsonValue::Undefined));

  if (dto.type == "textfield") {
    base.setData(Preference::TextData());
  } else if (dto.type == "password") {
    base.setData(Preference::PasswordData());
  } else if (dto.type == "checkbox") {
    base.setData(Preference::CheckboxData{.label = std::move(dto.label)});
  } else if (dto.type == "appPicker") {
    base.setData(Preference::AppPickerData());
  } else if (dto.type == "file") {
    base.setData(Preference::FilePickerData{.multiple = dto.multiple});
  } else if (dto.type == "directory") {
    base.setData(Preference::DirectoryPickerData{.multiple = dto.multiple});
  } else if (dto.type == "dropdown") {
    auto options = dto.data | std::views::as_rvalue |
                   std::views::transform([](manifest_dto::DropdownOption &&opt) {
                     return Preference::DropdownData::Option{.title = std::move(opt.title),
                                                             .value = std::move(opt.value)};
                   }) |
                   std::ranges::to<std::vector>();

    base.setData(Preference::DropdownData{std::move(options)});
  } else {
    qWarning() << "Unknown extension preference type" << dto.type;
  }

  return base;
}

static CommandArgument parseArgument(manifest_dto::Argument &&dto) {
  CommandArgument arg;

  if (dto.type == "text") arg.type = CommandArgument::Text;
  if (dto.type == "password") arg.type = CommandArgument::Password;
  if (dto.type == "dropdown") arg.type = CommandArgument::Dropdown;

  arg.name = std::move(dto.name);
  arg.placeholder = std::move(dto.placeholder);
  arg.required = dto.required;

  if (dto.type == "dropdown") {
    arg.data =
        dto.data | std::views::as_rvalue | std::views::transform([](manifest_dto::DropdownOption &&opt) {
          return CommandArgument::DropdownData{.title = std::move(opt.title), .value = std::move(opt.value)};
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

static ExtensionManifest::Command parseCommand(manifest_dto::Command &&dto) {
  ExtensionManifest::Command command;

  command.name = std::move(dto.name);
  command.title = std::move(dto.title);
  command.description = std::move(dto.description);
  command.defaultDisabled = dto.disabledByDefault;
  command.icon = std::move(dto.icon);

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

  command.keywords = std::move(dto.keywords);
  command.preferences = dto.preferences | std::views::as_rvalue | std::views::transform(parsePreference) |
                        std::ranges::to<std::vector>();
  command.arguments = dto.arguments | std::views::as_rvalue | std::views::transform(parseArgument) |
                      std::ranges::to<std::vector>();

  return command;
}

std::expected<ExtensionManifest, ManifestError> ExtensionManifest::fromPackageJson(const fs::path &path) {
  static const std::set<CommandMode> supportedModes{CommandMode::CommandModeView, CommandModeNoView};
  fs::path const manifestPath = path / "package.json";

  if (!fs::exists(manifestPath)) {
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
  manifest.name = std::move(pkg.name);
  manifest.title = std::move(pkg.title);
  manifest.description = std::move(pkg.description);
  manifest.icon = std::move(pkg.icon);
  manifest.author = std::move(pkg.author);
  manifest.needsRaycastApi = pkg.dependencies.contains(Omnicast::RAYCAST_NPM_API_PACKAGE.toStdString());

  if (manifest.id.startsWith("store.vicinae.")) {
    manifest.provenance = ExtensionManifest::Provenance::Vicinae;
  } else if (manifest.id.startsWith("store.raycast.")) {
    manifest.provenance = ExtensionManifest::Provenance::Raycast;
  } else {
    manifest.provenance = ExtensionManifest::Provenance::Local;
  }

  manifest.categories = std::move(pkg.categories);
  manifest.preferences = pkg.preferences | std::views::as_rvalue | std::views::transform(parsePreference) |
                         std::ranges::to<std::vector>();

  manifest.commands.reserve(pkg.commands.size());

  for (auto &dto : pkg.commands) {
    auto command = parseCommand(std::move(dto));

    command.provenance = manifest.provenance;
    command.entrypoint = path / std::format("{}.js", command.name.toStdString());

    if (supportedModes.contains(command.mode)) { manifest.commands.emplace_back(std::move(command)); }
  }

  return manifest;
}
