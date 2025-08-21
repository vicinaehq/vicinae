#include "vicinae.hpp"
#include "utils/utils.hpp"
#include <qprocess.h>

namespace fs = std::filesystem;

fs::path Omnicast::runtimeDir() {
  fs::path osRundir(QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation).toStdString());

  return osRundir / "vicinae";
}

fs::path Omnicast::dataDir() {
  return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation).toStdString();
}

fs::path Omnicast::configDir() {
  return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation).toStdString();
}

fs::path Omnicast::commandSocketPath() { return runtimeDir() / "vicinae.sock"; }
fs::path Omnicast::pidFile() { return runtimeDir() / "vicinae.pid"; }

std::vector<fs::path> Omnicast::xdgConfigDirs() {
  auto env = QProcessEnvironment::systemEnvironment();
  std::set<fs::path> seen;
  std::vector<fs::path> paths;
  fs::path configHome = homeDir() / ".config";

  if (auto value = env.value("XDG_CONFIG_HOME"); !value.isEmpty()) { configHome = value.toStdString(); }

  paths.emplace_back(configHome);
  seen.insert(configHome);

  for (const QString &dir : env.value("XDG_CONFIG_DIRS").split(':')) {
    fs::path path = dir.toStdString();

    if (std::ranges::contains(seen, path)) { continue; }

    seen.insert(path);
    paths.emplace_back(path);
  }

  return paths;
}
