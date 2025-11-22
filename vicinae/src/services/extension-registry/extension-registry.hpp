#pragma once
#include "common.hpp"
#include "preference.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "utils/expected.hpp"
#include <filesystem>
#include <qfilesystemwatcher.h>
#include <qjsonobject.h>
#include <qobject.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <vector>
#include <QString>

struct ManifestError {
  QString m_message;

  ManifestError(const QString &message) : m_message(message) {}
};

struct ExtensionManifest {
  enum class Provenance { Local, Vicinae, Raycast };

  struct Command {
    QString name;
    QString title;
    QString description;
    CommandMode mode;
    std::vector<Preference> preferences;
    std::vector<CommandArgument> arguments;
    std::optional<QString> icon;
    std::filesystem::path entrypoint;
    bool defaultDisabled;
    Provenance provenance;
  };

  std::filesystem::path path;
  QString id;
  QString name;
  QString title;
  QString description;
  QString icon;
  QString author;
  std::vector<QString> categories;
  std::vector<Preference> preferences;
  std::vector<Command> commands;
  bool needsRaycastApi = false;
  Provenance provenance;

  bool isFromRaycastStore() const { return provenance == ExtensionManifest::Provenance::Raycast; }
  bool isFromVicinaeStore() const { return provenance == ExtensionManifest::Provenance::Vicinae; }
  bool isLocal() const { return provenance == ExtensionManifest::Provenance::Local; }
};

class ExtensionRegistry : public QObject {
  Q_OBJECT

signals:
  void extensionAdded(const QString &id);
  void extensionUninstalled(const QString &id);

  // used to notify subscribers that they should rescan
  void extensionsChanged() const;

public:
  /**
   * Extension directory of the local user.
   * Store extensions are always installed in the local extension directory.
   */
  static std::filesystem::path localExtensionDir();

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

  tl::expected<ExtensionManifest, ManifestError> scanBundle(const std::filesystem::path &path);
  std::vector<ExtensionManifest> scanAll();

  void rescanBundle();
  bool isInstalled(const QString &id) const;
  bool uninstall(const QString &id);

  void requestScan() { emit extensionsChanged(); }

  CommandArgument parseArgumentFromObject(const QJsonObject &obj);
  Preference parsePreferenceFromObject(const QJsonObject &obj);
  ExtensionManifest::Command parseCommandFromObject(const QJsonObject &obj);

  /**
   * List of directories scanned for extensions bundles, in order.
   * An extension is uniquely identified by its directory name and the first occurence shadows
   * all subsequent ones.
   */
  std::span<const std::filesystem::path> extensionDirs() const;

  QTimer m_rescanDebounce;
  LocalStorageService &m_storage;
  QFileSystemWatcher *m_watcher = new QFileSystemWatcher(this);
  std::vector<std::filesystem::path> m_paths;

  // filename of every installed extension
  std::unordered_map<std::string, std::filesystem::path> m_installed;
};
