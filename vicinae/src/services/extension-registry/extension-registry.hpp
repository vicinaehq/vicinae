#pragma once
#include "services/local-storage/local-storage-service.hpp"
#include "extension-manifest.hpp"
#include <filesystem>
#include <qfilesystemwatcher.h>
#include <qjsonobject.h>
#include <expected>
#include <qobject.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <vector>
#include <QString>

class ExtensionRegistry : public QObject {
  Q_OBJECT

signals:
  void extensionAdded(const QString &id);
  void extensionUninstalled(const QString &id);

  // used to notify subscribers that they should rescan
  void extensionsChanged() const;

public:
  /**
   * List of directories scanned for extensions bundles, in order.
   * An extension is uniquely identified by its directory name and the first occurence shadows
   * all subsequent ones.
   */
  static std::vector<std::filesystem::path> extensionDirectories();

  /**
   * Extension directory of the local user.
   * Store extensions are always installed in the local extension directory.
   */
  static std::filesystem::path localExtensionDirectory();

  /**
   * Support directory used by extensions to store additional data at runtime.
   * This location is not related to the extension installation directory in any way.
   */
  static std::filesystem::path supportDirectory();

  /**
   * Support directory for extension with ID `id`.
   */
  static std::filesystem::path supportDirectory(const std::string &id);

  ExtensionRegistry(LocalStorageService &storage);

  /**
   * Unzip and install extension from a background thread.
   * The `extensionAdded` and `extensionsChanged` signals are emitted
   * once the bundle has been fully extracted out.
   * You can pass a callback if you want to do something (such as displaying a confirmation toast)
   * right after the unzipping was done.
   */
  QFuture<bool> installFromZip(const QString &id, std::string data, std::function<void(bool)> cb = {});

  bool isInstalled(const QString &id) const;
  bool uninstall(const QString &id);
  void requestScan() { emit extensionsChanged(); }
  std::vector<ExtensionManifest> scanAll();

private:
  QTimer m_rescanDebounce;
  LocalStorageService &m_storage;
  QFileSystemWatcher *m_watcher = new QFileSystemWatcher(this);
  std::vector<std::filesystem::path> m_extDirs;

  // filename of every installed extension
  std::unordered_map<std::string, std::filesystem::path> m_installed;
};
