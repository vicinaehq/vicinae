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
  std::filesystem::path extensionDir() const;

  QTimer m_rescanDebounce;
  LocalStorageService &m_storage;
  QFileSystemWatcher *m_watcher = new QFileSystemWatcher(this);
};
