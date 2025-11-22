#include "extension-registry.hpp"
#include "common.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "vicinae.hpp"
#include <QJsonArray>
#include "services/extension-registry/extension-registry.hpp"
#include "utils/utils.hpp"
#include "xdgpp/env/env.hpp"
#include "zip/unzip.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <filesystem>
#include <malloc.h>
#include <qfilesystemwatcher.h>
#include <QJsonParseError>
#include <qfuturewatcher.h>
#include <qlogging.h>
#include <qobjectdefs.h>
#include <ranges>

namespace fs = std::filesystem;

ExtensionRegistry::ExtensionRegistry(LocalStorageService &storage) : m_storage(storage) {
  using namespace std::chrono_literals;

  m_paths = xdgpp::dataDirs() | std::views::transform([](auto &&p) { return p / "vicinae" / "extensions"; }) |
            std::ranges::to<std::vector>();
  m_rescanDebounce.setInterval(100ms);
  m_rescanDebounce.setSingleShot(true);

  for (const auto &dir : extensionDirs()) {
    m_watcher->addPath(dir.c_str());
  }

  // XXX: we currently do not support removing extensions by filesystem removal
  // An extension should be removed from within Vicinae directly so that other cleanup tasks
  // can be performed.
  connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, [this]() { m_rescanDebounce.start(); });
  connect(&m_rescanDebounce, &QTimer::timeout, this, [this]() { requestScan(); });
}

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

QFuture<bool> ExtensionRegistry::installFromZip(const QString &id, std::string data,
                                                std::function<void(bool)> cb) {
  fs::path extractDir = localExtensionDir() / id.toStdString();
  auto future = QtConcurrent::run([id, data, extractDir]() {
    Unzipper unzip = std::string_view(data);

    if (!unzip) {
      qCritical() << "Failed to create unzipper";
      return false;
    }

    unzip.extract(extractDir, {.stripComponents = 1});
    return true;
  });

  auto watcher = new QFutureWatcher<bool>;

  watcher->setFuture(future);

  connect(watcher, &QFutureWatcher<bool>::finished, this, [this, id, cb, watcher = std::move(watcher)]() {
    if (watcher->isCanceled()) return;
    auto result = watcher->result();
    if (cb) { cb(result); }
    if (result) {
      emit extensionAdded(id);
      emit extensionsChanged();
    }
    watcher->deleteLater();
  });

  return future;
}

bool ExtensionRegistry::uninstall(const QString &id) {
  std::string sid = id.toStdString();
  auto it = m_installed.find(sid);

  if (it == m_installed.end()) {
    qWarning() << "Tried to uninstall extension with id" << id << "but could not find any installation of it";
    return false;
  }

  auto &bundle = it->second;
  std::error_code ec;

  if (!fs::is_directory(bundle, ec)) {
    qDebug() << "could not remove extension bundle at" << bundle << "not a directory";
    return false;
  }

  fs::remove_all(bundle, ec);
  fs::remove_all(supportDirectory(sid), ec);
  m_installed.erase(it);
  m_storage.clearNamespace(id);
  emit extensionUninstalled(id);

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
  } else {
    qWarning() << "Unknown extension preference type" << type;
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

fs::path ExtensionRegistry::localExtensionDir() { return xdgpp::dataHome() / "vicinae" / "extensions"; }

fs::path ExtensionRegistry::supportDirectory() { return xdgpp::dataHome() / "vicinae" / "support"; }

fs::path ExtensionRegistry::supportDirectory(const std::string &id) { return supportDirectory() / id; }

std::span<const fs::path> ExtensionRegistry::extensionDirs() const { return m_paths; }

std::vector<ExtensionManifest> ExtensionRegistry::scanAll() {
  std::error_code ec;
  std::vector<ExtensionManifest> manifests;

  manifests.reserve(m_installed.size());
  m_installed.clear();

  for (const fs::path &path : extensionDirs()) {
    for (const auto &entry : fs::directory_iterator(path, ec)) {
      if (!entry.is_directory(ec)) continue;

      fs::path path = entry.path();
      std::string filename = path.filename();

      if (filename.starts_with('.')) continue;

      if (auto it = m_installed.find(filename); it != m_installed.end()) {
        qWarning() << path.c_str()
                   << "shadowed by extension with same directory name in higher precedence directory"
                   << it->second;
        continue;
      }

      auto manifest = scanBundle(entry.path());

      if (!manifest) {
        qCritical() << "Failed to load bundle at" << entry.path().c_str() << manifest.error().m_message;
        continue;
      }

      m_installed.insert({filename, path});
      manifests.emplace_back(manifest.value());
    }
  }

  return manifests;
}

bool ExtensionRegistry::isInstalled(const QString &id) const {
  return m_installed.contains(id.toStdString());
}

tl::expected<ExtensionManifest, ManifestError> ExtensionRegistry::scanBundle(const fs::path &path) {
  static const std::set<CommandMode> supportedModes{CommandMode::CommandModeView, CommandModeNoView};
  fs::path manifestPath = path / "package.json";

  if (!fs::exists(manifestPath)) {
    return tl::unexpected<ManifestError>(
        QString("Could not find package.json file at %1").arg(manifestPath.c_str()));
  }

  QFile file(manifestPath);

  if (!file.open(QIODevice::ReadOnly)) {
    return tl::unexpected<ManifestError>(QString("Failed to open %1 for read").arg(manifestPath.c_str()));
  }

  QJsonParseError jsonError;
  auto json = QJsonDocument::fromJson(file.readAll(), &jsonError);

  if (jsonError.error) {
    return tl::unexpected<ManifestError>(
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
