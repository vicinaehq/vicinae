#pragma once
#include <memory>
#include <qfilesystemwatcher.h>
#include <qlogging.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qsqlquery.h>
#include <qtmetamacros.h>
#include "abstract-app-db.hpp"
#include "common.hpp"
#include "omni-database.hpp"

class AppService : public QObject, public NonCopyable {
  Q_OBJECT

public:
  std::vector<std::filesystem::path> m_additionalSearchPaths;

private:
  QFileSystemWatcher *m_watcher = new QFileSystemWatcher(this);
  OmniDatabase &m_db;
  std::unique_ptr<AbstractAppDatabase> m_provider;
  std::optional<QString> m_prefix;

  static std::unique_ptr<AbstractAppDatabase> createLocalProvider();
  std::vector<std::filesystem::path> mergedPaths() const;

  bool reinstallWatches(const std::vector<std::filesystem::path> &paths);
  void handleDirectoryChanged(const QString &path);

public:
  /**
   * Concrete implementation for the underlying system.
   */
  AbstractAppDatabase *provider() const;
  std::vector<std::shared_ptr<AbstractApplication>> list() const;

  /**
   * Launch application with the provided set of arguments. If the application
   * runs in a terminal, the default system terminal will be spawned.
   */
  bool launch(const AbstractApplication &app, const std::vector<QString> &args = {}) const;

  bool launchTerminalCommand(const std::vector<QString> &cmdLine,
                             const LaunchTerminalCommandOptions &opts = {});

  /**
   * Launch a new process using arbitrary prog name and args. No expansion of any kind
   * will be performed.
   */
  bool launchRaw(const QString &prog, const std::vector<QString> &args);

  /**
   * Custom launcher that will be used to invoke all the applications.
   * Typically used to integrate with programs such as uwsm: https://github.com/Vladimir-csp/uwsm
   */
  void setLaunchPrefix(const std::optional<QString> &prefix) { m_prefix = prefix; }

  std::vector<std::filesystem::path> defaultSearchPaths() const;

  /**
   * Returns the default terminal emulator or a null pointer if none is available.
   */
  std::shared_ptr<AbstractApplication> terminalEmulator() const;
  std::shared_ptr<AbstractApplication> textEditor() const;
  std::shared_ptr<AbstractApplication> webBrowser() const;
  std::shared_ptr<AbstractApplication> fileBrowser() const;

  std::shared_ptr<AbstractApplication> findById(const QString &id) const;
  std::shared_ptr<AbstractApplication> findByClass(const QString &wmClass) const;

  /**
   * Attempts to find an application from the provided string
   * The following fields are searched, in order:
   * - application id (for non-apple UNIXes, with or without the .desktop extension)
   * - window manager class, if applicable
   */
  std::shared_ptr<AbstractApplication> find(const QString &target) const;
  std::shared_ptr<AbstractApplication> findDefaultOpener(const QString &target) const;
  void setAdditionalSearchPaths(const std::vector<std::filesystem::path> &paths);

  bool openTarget(const QString &target) const;
  bool openTarget(const QUrl &target) const;

  /**
   * Scan application directories synchronously.
   * This is usually very fast.
   */
  bool scanSync();

  std::vector<std::shared_ptr<AbstractApplication>> findOpeners(const QString &target) const;

  /**
   * Calls findOpeners and applies additonal dedupe logic to the list of results.
   * For instance, it will get rid of entries that have a similar name, although they are different
   * applications on a technical perspective.
   */
  std::vector<std::shared_ptr<AbstractApplication>> findCuratedOpeners(const QString &target) const;

  AppService(OmniDatabase &db);

signals:
  void appsChanged() const;
};
