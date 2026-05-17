#include "vicinae.hpp"
#include <qcoreapplication.h>
#include <qlogging.h>
#include <qprocess.h>
#include <QProcessEnvironment>
#include <ranges>
#include <set>

#ifndef Q_OS_MACOS
#include "xdgpp/env/env.hpp"
#endif

namespace fs = std::filesystem;

fs::path Omnicast::runtimeDir() {
#ifdef Q_OS_MACOS
  if (const char *t = std::getenv("TMPDIR")) return fs::path(t) / "vicinae";
  return "/tmp/vicinae";
#else
  return fs::path(QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation).toStdString()) /
         "vicinae";
#endif
}

fs::path Omnicast::dataDir() {
#ifdef Q_OS_MACOS
  return fs::path(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation).toStdString()) /
         "vicinae";
#else
  return xdgpp::dataHome() / "vicinae";
#endif
}

fs::path Omnicast::configDir() {
#ifdef Q_OS_MACOS
  return dataDir();
#else
  return xdgpp::configHome() / "vicinae";
#endif
}

fs::path Omnicast::stateDir() {
#ifdef Q_OS_MACOS
  return dataDir();
#else
  return xdgpp::stateHome() / "vicinae";
#endif
}

fs::path Omnicast::bundleResourceDir() {
#ifdef Q_OS_MACOS
  // Inside Vicinae.app/Contents/MacOS/<exe>, Resources is one level up.
  auto const appDir = QCoreApplication::applicationDirPath().toStdString();
  return fs::path(appDir).parent_path() / "Resources";
#else
  // Linux: rely on the install-prefix share dir. Empty if running uninstalled —
  // callers should layer their own fallbacks (e.g. xdgpp::dataDirs()).
  return {};
#endif
}

fs::path Omnicast::commandSocketPath() { return runtimeDir() / "vicinae.sock"; }
fs::path Omnicast::pidFile() { return runtimeDir() / "vicinae.pid"; }

void Omnicast::ensureDirectories() {
  for (auto const &dir : {runtimeDir(), dataDir(), stateDir(), configDir()}) {
    std::error_code ec;
    fs::create_directories(dir, ec);
    if (ec) { qWarning() << "Failed to create directory" << dir.c_str() << ec.message(); }
  }
}

std::vector<fs::path> Omnicast::systemPaths() {
  const char *path = getenv("PATH");

  if (!path) return {};

  std::set<fs::path> seen;
  std::vector<fs::path> paths;

  for (const auto &part : std::views::split(std::string_view(path), std::string_view(":"))) {
    fs::path const path = std::string_view(part.begin(), part.end());

    if (seen.contains(path)) continue;

    seen.insert(path);
    paths.emplace_back(path);
  }

  return paths;
}
