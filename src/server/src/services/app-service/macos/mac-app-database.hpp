#pragma once
#include "mac-app.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include <QCoreApplication>
#include <memory>
#include <unordered_map>
#include <vector>

class MacAppDatabase : public AbstractAppDatabase {
  Q_DECLARE_TR_FUNCTIONS(MacAppDatabase)

public:
  MacAppDatabase();

  std::vector<std::filesystem::path> defaultSearchPaths() const override;
  std::vector<std::filesystem::path> searchPaths() const override;
  bool scan(const std::vector<std::filesystem::path> &paths) override;

  PreferenceList preferences() const override;
  void applyPreferences(const QJsonObject &preferences) override;

  bool launch(const AbstractApplication &exec, const std::vector<QString> &args = {}) const override;
  bool launchTerminalCommand(const std::vector<QString> &cmdline,
                             const LaunchTerminalCommandOptions &opts = {}) const override;

  std::vector<AppPtr> findOpeners(const Target &target) const override;
  AppPtr findDefaultOpener(const Target &target) const override;
  AppPtr findById(const QString &id) const override;
  std::vector<AppPtr> list() const override;
  AppPtr findByClass(const QString &name) const override;

  AppPtr fileBrowser() const override;
  AppPtr genericTextEditor() const override;
  AppPtr webBrowser() const override;
  AppPtr terminalEmulator() const override;
  bool showInFileBrowser(const std::filesystem::path &path, bool select) const override;
  bool openLocation(const AbstractApplication &app) const override;
  AppPtr locationOpener(const AbstractApplication &app) const override;

private:
  std::vector<std::shared_ptr<MacApplication>> m_apps;
  std::unordered_map<QString, std::shared_ptr<MacApplication>> m_appsById;
  std::vector<std::filesystem::path> m_extraSearchPaths;
};
