#include "extension-registry.hpp"
#include "common.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "vicinae.hpp"
#include <QJsonArray>
#include "services/extension-registry/extension-registry.hpp"
#include "utils/utils.hpp"
#include "zip/unzip.hpp"
#include <filesystem>
#include <qfilesystemwatcher.h>
#include <qjsonparseerror.h>
#include <qlogging.h>

namespace fs = std::filesystem;

ExtensionRegistry::ExtensionRegistry(OmniCommandDatabase &commandDb, LocalStorageService &storage)
    : m_db(commandDb), m_storage(storage) {
  m_watcher->addPath(extensionDir().c_str());

  // XXX: we currently do not support removing extensions by filesystem removal
  // An extension should be removed from within Vicinae directly so that other cleanup tasks
  // can be performed.
  connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, [this]() { requestScan(); });
}

fs::path ExtensionRegistry::extensionDir() const { return Omnicast::dataDir() / "extensions"; }

CommandArgument ExtensionRegistry::parseArgumentFromObject(const QJsonObject &obj) {
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

bool ExtensionRegistry::installFromZip(const QString &id, std::string_view data) {
  fs::path extractDir = extensionDir() / id.toStdString();
  Unzipper unzip(data);

  if (!unzip) {
    qCritical() << "Failed to create unzipper";
    return false;
  }

  unzip.extract(extractDir, {.stripComponents = 1});
  emit extensionAdded(id);
  emit extensionsChanged();

  return true;
}

bool ExtensionRegistry::uninstall(const QString &id) {
  fs::path bundle = extensionDir() / id.toStdString();

  if (!fs::is_directory(bundle)) return false;

  fs::remove_all(bundle);
  m_storage.clearNamespace(id);

  emit extensionUninstalled(id);
  emit extensionsChanged();

  return true;
}

Preference ExtensionRegistry::parsePreferenceFromObject(const QJsonObject &obj) {
  auto type = obj["type"].toString();
  Preference base;

  base.setTitle(obj["title"].toString());
  base.setDescription(obj["description"].toString());
  base.setName(obj["name"].toString());
  base.setPlaceholder(obj["placeholder"].toString());
  base.setRequired(obj["required"].toBool());
  base.setDefaultValue(obj.value("default"));

  if (type == "textfield") { base.setData(Preference::TextData()); }
  if (type == "password") { base.setData(Preference::PasswordData()); }

  if (type == "checkbox") {
    auto checkbox = Preference::CheckboxData(obj["label"].toString());

    base.setData(checkbox);
  }

  if (type == "appPicker") {
    // XXX: implement later - if we really need it
  }

  if (type == "dropdown") {
    auto data = obj["data"].toArray();
    std::vector<Preference::DropdownData::Option> options;

    options.reserve(data.size());

    for (const auto &child : data) {
      auto obj = child.toObject();

      options.push_back({.title = obj["title"].toString(), .value = obj["value"].toString()});
    }

    base.setData(Preference::DropdownData{options});
  }

  return base;
}

ExtensionManifest::Command ExtensionRegistry::parseCommandFromObject(const QJsonObject &obj) {
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

std::vector<ExtensionManifest> ExtensionRegistry::scanAll() {
  std::error_code ec;
  std::vector<ExtensionManifest> manifests;

  for (const auto &entry : fs::directory_iterator(extensionDir(), ec)) {
    if (!entry.is_directory()) continue;

    auto manifest = scanBundle(entry.path());

    if (!manifest) {
      qCritical() << "Failed to load bundle at" << entry.path().c_str() << manifest.error().m_message;
      continue;
    }

    manifests.emplace_back(manifest.value());
  }

  return manifests;
}

bool ExtensionRegistry::isInstalled(const QString &id) const {
  return fs::is_directory(extensionDir() / id.toStdString());
}

std::expected<ExtensionManifest, ManifestError> ExtensionRegistry::scanBundle(const fs::path &path) {
  static const std::vector<CommandMode> supportedModes{CommandMode::CommandModeView, CommandModeNoView};
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

  manifest.path = path;
  manifest.id = QString::fromStdString(getLastPathComponent(path));
  manifest.name = obj.value("name").toString();
  manifest.title = obj.value("title").toString();
  manifest.description = obj.value("description").toString();
  manifest.icon = obj.value("icon").toString();
  manifest.author = obj.value("author").toString();

  for (const auto &obj : obj.value("categories").toArray()) {
    manifest.categories.emplace_back(obj.toString());
  }

  for (const auto &obj : obj.value("commands").toArray()) {
    auto command = parseCommandFromObject(obj.toObject());

    command.entrypoint = path / std::format("{}.js", command.name.toStdString());

    if (std::ranges::contains(supportedModes, command.mode)) { manifest.commands.emplace_back(command); }
  }

  for (const auto &obj : obj.value("preferences").toArray()) {
    manifest.preferences.emplace_back(parsePreferenceFromObject(obj.toObject()));
  }

  return manifest;
}
