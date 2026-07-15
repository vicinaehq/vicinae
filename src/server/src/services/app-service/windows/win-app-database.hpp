#pragma once
#include "services/app-service/abstract-app-db.hpp"
#include "win-app.hpp"
#include <QFileSystemWatcher>
#include <QString>
#include <QTimer>
#include <atomic>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct UwpPackageWatcher;

// App provider merging two tracks: Win32 shortcuts/registry and packaged apps (WinRT PackageManager).
class WindowsAppDatabase : public AbstractAppDatabase {
public:
  WindowsAppDatabase();
  ~WindowsAppDatabase() override;

  std::vector<std::filesystem::path> defaultSearchPaths() const override;
  bool scan(const std::vector<std::filesystem::path> &paths) override;

  bool launch(const AbstractApplication &exec, const std::vector<QString> &args = {}) const override;
  bool launchTerminalCommand(const std::vector<QString> &cmdline,
                             const LaunchTerminalCommandOptions &opts = {}) const override;

  std::vector<AppPtr> findOpeners(const Target &target) const override;
  AppPtr findDefaultOpener(const Target &target) const override;
  AppPtr findById(const QString &id) const override;
  std::vector<AppPtr> list() const override;
  AppPtr findByClass(const QString &name) const override;
  PreferenceList preferences() const override;

  AppPtr fileBrowser() const override;
  AppPtr genericTextEditor() const override;
  AppPtr webBrowser() const override;
  AppPtr terminalEmulator() const override;
  bool showInFileBrowser(const std::filesystem::path &path, bool select) const override;
  bool openLocation(const AbstractApplication &app) const override;
  AppPtr locationOpener(const AbstractApplication &app) const override;

private:
  void scanWin32(const std::vector<std::filesystem::path> &paths); // recursive
  void scanDesktop();                                              // non-recursive
  void scanAppPaths();
  void scanUwp();
  void refreshUwpCache();
  void installWatches();
  void addShortcut(const std::filesystem::path &file);
  void addApp(std::shared_ptr<WindowsApplication> app);
  void indexAliases(const std::shared_ptr<WindowsApplication> &app);

  AppPtr appForExecutable(const std::filesystem::path &exe, const QString &name,
                          const QString &openerExtension = {}) const;
  AppPtr resolveExecutable(const std::filesystem::path &exe, const QString &name,
                           const QString &openerExtension = {}) const;

  AppPtr makeShellOpenApp(const QString &target) const;

  std::vector<std::shared_ptr<WindowsApplication>> m_apps;
  std::unordered_map<QString, std::shared_ptr<WindowsApplication>> m_appsById;
  // lowercased exe target paths and AUMIDs, for findByClass
  std::unordered_map<QString, std::shared_ptr<WindowsApplication>> m_appsByAlias;
  std::unique_ptr<UwpPackageWatcher> m_uwpWatcher;
  std::vector<std::shared_ptr<WindowsApplication>> m_uwpCache;
  std::atomic<bool> m_uwpDirty = true; // set from WinRT event threads
  std::unordered_set<QString> m_watchDirs;
  QFileSystemWatcher m_watcher;
  QTimer m_rescanTimer;
};
