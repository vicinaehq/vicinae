#include "extension-registry.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include <QJsonArray>
#include "services/extension-registry/extension-registry.hpp"
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

  m_extDirs = extensionDirectories();
  m_rescanDebounce.setInterval(100ms);
  m_rescanDebounce.setSingleShot(true);

  for (const auto &dir : m_extDirs) {
    m_watcher->addPath(dir.c_str());
  }

  // XXX: we currently do not support removing extensions by filesystem removal
  // An extension should be removed from within Vicinae directly so that other cleanup tasks
  // can be performed.
  connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, [this]() { m_rescanDebounce.start(); });
  connect(&m_rescanDebounce, &QTimer::timeout, this, [this]() { requestScan(); });
}

QFuture<bool> ExtensionRegistry::installFromZip(const QString &id, std::string data,
                                                std::function<void(bool)> cb) {
  fs::path extractDir = localExtensionDirectory() / id.toStdString();
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
  emit extensionsChanged();

  return true;
}

fs::path ExtensionRegistry::localExtensionDirectory() { return xdgpp::dataHome() / "vicinae" / "extensions"; }

fs::path ExtensionRegistry::supportDirectory() { return xdgpp::dataHome() / "vicinae" / "support"; }

fs::path ExtensionRegistry::supportDirectory(const std::string &id) { return supportDirectory() / id; }

std::vector<fs::path> ExtensionRegistry::extensionDirectories() {
  std::vector<fs::path> paths;
  auto dd = xdgpp::dataDirs();
  auto local = localExtensionDirectory();

  paths.reserve(dd.size() + 1);
  paths.push_back(local); // we always consider local directory first, no matter what's
                          // in XDG_DATA_DIRS

  for (const auto &dir : dd) {
    fs::path extDir = dir / "vicinae" / "extensions";
    if (extDir != local) paths.emplace_back(extDir);
  }

  return paths;
}

std::vector<ExtensionManifest> ExtensionRegistry::scanAll() {
  std::error_code ec;
  std::vector<ExtensionManifest> manifests;

  manifests.reserve(m_installed.size());
  m_installed.clear();

  for (const fs::path &path : m_extDirs) {
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

      auto manifest = ExtensionManifest::fromPackageJson(entry.path());

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
