#pragma once
#include "services/app-service/abstract-app-db.hpp"
#include <xdgpp/desktop-entry/file.hpp>
#include "xdg-app.hpp"
#include <qfileinfo.h>
#include <qlogging.h>
#include <qmimedatabase.h>
#include <qmimetype.h>
#include <qobjectdefs.h>
#include <qprocess.h>
#include <xdgpp/xdgpp.hpp>

class XdgAppDatabase : public AbstractAppDatabase {
public:
  bool scan(const std::vector<std::filesystem::path> &paths) override;
  std::vector<std::filesystem::path> defaultSearchPaths() const override;
  AppPtr findByClass(const QString &name) const override;
  AppPtr findDefaultOpener(const QString &target) const override;
  std::vector<AppPtr> findOpeners(const QString &mime) const override;
  AppPtr findById(const QString &id) const override;
  std::vector<AppPtr> list() const override;
  std::vector<AppPtr> findOpeners(const QString &mimeName);
  bool launch(const AbstractApplication &exec, const std::vector<QString> &args = {},
              const std::optional<QString> &launchPrefix = {}) const override;

  bool launchTerminalCommand(const std::vector<QString> &cmdline,
                             const LaunchTerminalCommandOptions &opts = {},
                             const std::optional<QString> &prefix = {}) const override;

  AppPtr terminalEmulator() const override;
  AppPtr fileBrowser() const override;
  AppPtr genericTextEditor() const override;
  AppPtr webBrowser() const override;

  XdgAppDatabase();

private:
  bool launchProcess(const QString &prog, const QStringList args,
                     const std::optional<std::filesystem::path> &workingDirectory) const;

  xdgpp::DesktopEntry::TerminalExec getTermExec(const XdgApplication &app) const;
  xdgpp::DesktopEntry::TerminalExec inferTermExec(const XdgApplication &app) const;

  AppPtr defaultForMime(const QString &mime) const;
  std::vector<AppPtr> findAssociations(const QString &mime) const;
  QString mimeNameForTarget(const QString &target) const;
  AppPtr findByCategory(const QString &category) const;

  std::unordered_map<QString, std::shared_ptr<AbstractApplication>> appMap;
  std::vector<xdgpp::MimeAppsListFile> m_mimeAppsLists;
  QMimeDatabase m_mimeDb;
  std::vector<std::shared_ptr<XdgApplication>> m_apps;

  // apps segmented by data dir (needed for association resolution)
  std::unordered_map<std::filesystem::path, std::vector<std::shared_ptr<XdgApplication>>> m_dataDirToApps;
};
