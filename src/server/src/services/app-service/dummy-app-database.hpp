#pragma once
#include "services/app-service/abstract-app-db.hpp"

class DummyAppDatabase : public AbstractAppDatabase {
public:
  std::vector<std::filesystem::path> defaultSearchPaths() const override { return {}; }
  bool scan(const std::vector<std::filesystem::path> &) override { return true; }
  bool launch(const AbstractApplication &, const std::vector<QString> & = {}) const override { return false; }
  bool launchTerminalCommand(const std::vector<QString> &,
                             const LaunchTerminalCommandOptions & = {}) const override {
    return false;
  }
  std::vector<AppPtr> findOpeners(const Target &) const override { return {}; }
  AppPtr findDefaultOpener(const Target &) const override { return nullptr; }
  AppPtr findById(const QString &) const override { return nullptr; }
  std::vector<AppPtr> list() const override { return {}; }
  AppPtr findByClass(const QString &) const override { return nullptr; }
  AppPtr fileBrowser() const override { return nullptr; }
  AppPtr genericTextEditor() const override { return nullptr; }
  AppPtr webBrowser() const override { return nullptr; }
  AppPtr terminalEmulator() const override { return nullptr; }
  bool openLocation(const AbstractApplication &) const override { return false; }
  AppPtr locationOpener(const AbstractApplication &) const override { return nullptr; }
  bool showInFileBrowser(const std::filesystem::path &, bool) const override { return false; }
};
