#include "extension-manifest.hpp"
#include "utils.hpp"
#include "vicinae.hpp"
#include <QJsonObject>
#include <QJsonDocument>

namespace fs = std::filesystem;

std::expected<ExtensionManifest, ManifestError> ExtensionManifest::fromPackageJson(const fs::path &path) {
  static const std::set<CommandMode> supportedModes{CommandMode::CommandModeView, CommandModeNoView};
  fs::path manifestPath = path / "package.json";

  if (!fs::exists(manifestPath)) {
    return std::unexpected<ManifestError>(
        QString("Could not find package.json file at %1").arg(manifestPath.c_str()));
  }

  QFile file(manifestPath);

  if (!file.open(QIODevice::ReadOnly)) {
    return std::unexpected<ManifestError>(QString("Failed to open %1 for read").arg(manifestPath.c_str()));
  }

  QJsonParseError jsonError;
  auto json = QJsonDocument::fromJson(file.readAll(), &jsonError);

  if (jsonError.error) {
    return std::unexpected<ManifestError>(
        QString("Failed to parse package.json at %1").arg(manifestPath.c_str()));
  }

  ExtensionManifest manifest;
  auto obj = json.object();
  auto deps = obj.value("dependencies").toObject();

  manifest.path = path;
  manifest.id = QString::fromStdString(getLastPathComponent(path));
  manifest.name = obj.value("name").toString();
  manifest.title = obj.value("title").toString();
  manifest.description = obj.value("description").toString();
  manifest.icon = obj.value("icon").toString();
  manifest.author = obj.value("author").toString();
  manifest.needsRaycastApi = deps.contains(Omnicast::RAYCAST_NPM_API_PACKAGE);

  if (manifest.id.startsWith("store.vicinae.")) {
    manifest.provenance = ExtensionManifest::Provenance::Vicinae;
  } else if (manifest.id.startsWith("store.raycast.")) {
    manifest.provenance = ExtensionManifest::Provenance::Raycast;
  } else {
    manifest.provenance = ExtensionManifest::Provenance::Local;
  }

  for (const auto &obj : obj.value("categories").toArray()) {
    manifest.categories.emplace_back(obj.toString());
  }

  for (const auto &obj : obj.value("commands").toArray()) {
    auto command = parseCommandFromObject(obj.toObject());

    command.provenance = manifest.provenance;
    command.entrypoint = path / std::format("{}.js", command.name.toStdString());

    if (supportedModes.contains(command.mode)) { manifest.commands.emplace_back(command); }
  }

  for (const auto &obj : obj.value("preferences").toArray()) {
    manifest.preferences.emplace_back(parsePreferenceFromObject(obj.toObject()));
  }

  return manifest;
}

Preference ExtensionManifest::parsePreferenceFromObject(const QJsonObject &obj) {
  auto type = obj["type"].toString();
  Preference base;

  base.setTitle(obj["title"].toString());
  base.setDescription(obj["description"].toString());
  base.setName(obj["name"].toString());
  base.setPlaceholder(obj["placeholder"].toString());
  base.setRequired(obj["required"].toBool());
  base.setDefaultValue(obj.value("default"));

  if (type == "textfield") {
    base.setData(Preference::TextData());
  } else if (type == "password") {
    base.setData(Preference::PasswordData());
  } else if (type == "checkbox") {
    auto checkbox = Preference::CheckboxData(obj["label"].toString());
    base.setData(checkbox);
  } else if (type == "appPicker") {
    base.setData(Preference::AppPickerData());
  } else if (type == "file") {
    base.setData(Preference::FilePickerData());
  } else if (type == "directory") {
    base.setData(Preference::DirectoryPickerData());
  } else if (type == "dropdown") {
    auto data = obj["data"].toArray();
    std::vector<Preference::DropdownData::Option> options;

    options.reserve(data.size());

    for (const auto &child : data) {
      auto obj = child.toObject();

      options.push_back({.title = obj["title"].toString(), .value = obj["value"].toString()});
    }

    base.setData(Preference::DropdownData{options});

    // For dropdown, validate default value: use provided value if valid, otherwise use first option
    if (!options.empty()) {
      QJsonValue providedDefault = obj.value("default");
      QString defaultValue = options.front().value; // Default to first option

      if (providedDefault.isString()) {
        QString providedValue = providedDefault.toString();
        for (const auto &option : options) {
          if (option.value == providedValue) {
            defaultValue = providedValue;
            break;
          }
        }
      }

      base.setDefaultValue(defaultValue);
    }
  } else {
    qWarning() << "Unknown extension preference type" << type;
  }

  return base;
}

CommandArgument ExtensionManifest::parseArgumentFromObject(const QJsonObject &obj) {
  CommandArgument arg;
  QString type = obj.value("type").toString();

  if (type == "text") arg.type = CommandArgument::Text;
  if (type == "password") arg.type = CommandArgument::Password;
  if (type == "dropdown") arg.type = CommandArgument::Dropdown;

  arg.name = obj.value("name").toString();
  arg.placeholder = obj.value("placeholder").toString();
  arg.required = obj.value("required").toBool(false);

  if (type == "dropdown") {
    auto data = obj.value("data").toArray();
    std::vector<CommandArgument::DropdownData> options;

    options.reserve(data.size());

    for (const auto &child : data) {
      auto obj = child.toObject();

      options.push_back({.title = obj["title"].toString(), .value = obj["value"].toString()});
    }

    arg.data = options;
  }

  return arg;
}

ExtensionManifest::Command ExtensionManifest::parseCommandFromObject(const QJsonObject &obj) {
  ExtensionManifest::Command command;
  auto type = obj.value("mode");

  command.name = obj.value("name").toString();
  command.title = obj.value("title").toString();
  command.description = obj.value("description").toString();
  command.defaultDisabled = obj.value("disabledByDefault").toBool(false);

  if (obj.contains("icon")) { command.icon = obj.value("icon").toString(); }

  if (type == "view") {
    command.mode = CommandMode::CommandModeView;
  } else if (type == "no-view") {
    command.mode = CommandMode::CommandModeNoView;
  } else {
    command.mode = CommandMode::CommandModeInvalid;
  }

  for (const auto &obj : obj.value("preferences").toArray()) {
    command.preferences.emplace_back(parsePreferenceFromObject(obj.toObject()));
  }

  for (const auto &obj : obj.value("arguments").toArray()) {
    command.arguments.emplace_back(parseArgumentFromObject(obj.toObject()));
  }

  return command;
}
