#pragma once
#include "abstract-app-db.hpp"
#include "common.hpp"
#include "omni-database.hpp"
#include <memory>
#include <qlogging.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qsqlquery.h>
#include <qtmetamacros.h>
#include <QFileSystemWatcher>

class AppService : public QObject, public NonCopyable {
  Q_OBJECT

public:
  std::vector<std::filesystem::path> m_additionalSearchPaths;

private:
  QFileSystemWatcher *m_watcher = new QFileSystemWatcher(this);
  OmniDatabase &m_db;
  std::unique_ptr<AbstractAppDatabase> m_provider;

  static std::unique_ptr<AbstractAppDatabase> createLocalProvider();
  std::vector<std::filesystem::path> mergedPaths() const;

  bool reinstallWatches(const std::vector<std::filesystem::path> &paths);
  void handleDirectoryChanged(const QString &path);

public:
  /**
   * Concrete implementation for the underlying system.
   */
  AbstractAppDatabase *provider() const;
  std::vector<std::shared_ptr<Application>> list() const;

  /**
   * Launch application with the provided set of arguments. If the application
   * runs in a terminal, the default system terminal will be spawned.
   */
  bool launch(const Application &app, const std::vector<QString> &args = {}) const;

  /**
   * Launch a new process using arbitrary prog name and args. No expansion of any kind
   * will be performed.
   */
  bool launchRaw(const QString &prog, const std::vector<QString> &args);

  std::vector<std::filesystem::path> defaultSearchPaths() const;

  /**
   * Returns the default terminal emulator or a null pointer if none is available.
   */
  std::shared_ptr<Application> terminalEmulator() const;
  std::shared_ptr<Application> textEditor() const;
  std::shared_ptr<Application> webBrowser() const;
  std::shared_ptr<Application> fileBrowser() const;

  std::shared_ptr<Application> findById(const QString &id) const;
  std::shared_ptr<Application> findByClass(const QString &wmClass) const;

  /**
   * Attempts to find an application from the provided string
   * The following fields are searched, in order:
   * - application id (for non-apple UNIXes, with or without the .desktop extension)
   * - window manager class, if applicable
   */
  std::shared_ptr<Application> find(const QString &target) const;
  std::shared_ptr<Application> findBestOpener(const QString &target) const;
  void setAdditionalSearchPaths(const std::vector<std::filesystem::path> &paths);

  bool openTarget(const QString &target) const;

  /**
   * Scan application directories synchronously.
   * This is usually very fast.
   */
  bool scanSync();

  std::vector<std::shared_ptr<Application>> findOpeners(const QString &target) const;

  AppService(OmniDatabase &db);

signals:
  void appsChanged() const;
};
