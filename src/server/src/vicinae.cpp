#include "vicinae.hpp"
#include "common/common.hpp"
#include "utils.hpp"
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

fs::path Omnicast::runtimeDir() { return vicinae::runtimeDir(); }

fs::path Omnicast::dataDir() {
#ifdef Q_OS_MACOS
  return homeDir() / ".local" / "share" / "vicinae";
#else
  return xdgpp::dataHome() / "vicinae";
#endif
}

fs::path Omnicast::configDir() {
#ifdef Q_OS_MACOS
  return homeDir() / ".config" / "vicinae";
#else
  return xdgpp::configHome() / "vicinae";
#endif
}

fs::path Omnicast::stateDir() {
#ifdef Q_OS_MACOS
  return homeDir() / ".local" / "state" / "vicinae";
#else
  return xdgpp::stateHome() / "vicinae";
#endif
}

fs::path Omnicast::cacheDir() {
#ifdef Q_OS_MACOS
  return homeDir() / ".cache" / "vicinae";
#else
  return xdgpp::cacheHome() / "vicinae";
#endif
}

fs::path Omnicast::bundleResourceDir() {
#ifdef Q_OS_MACOS
  // Inside Vicinae.app/Contents/MacOS/<exe>, Resources is one level up.
  auto const appDir = QCoreApplication::applicationDirPath().toStdString();
  return fs::path(appDir).parent_path() / "Resources";
#else
  return {};
#endif
}

std::vector<fs::path> Omnicast::systemDataDirs() {
  std::vector<fs::path> paths;
#ifdef Q_OS_MACOS
  if (auto bundle = bundleResourceDir(); !bundle.empty()) { paths.emplace_back(std::move(bundle)); }
#else
  auto const dd = xdgpp::dataDirs();
  paths.reserve(dd.size());
  for (auto const &dir : dd) {
    paths.emplace_back(dir / "vicinae");
  }
#endif
  return paths;
}

std::vector<fs::path> Omnicast::dataSearchPaths(std::string_view subdir) {
  std::vector<fs::path> paths;
  auto const sub = fs::path(subdir);
  auto const user = dataDir() / sub;

  paths.reserve(1 + systemDataDirs().size());
  paths.emplace_back(user);
  for (auto const &dir : systemDataDirs()) {
    auto p = dir / sub;
    if (p != user) paths.emplace_back(std::move(p));
  }
  return paths;
}

fs::path Omnicast::commandSocketPath() { return vicinae::serverSocketPath(); }
fs::path Omnicast::pidFile() { return runtimeDir() / "vicinae.pid"; }

void Omnicast::ensureDirectories() {
  for (auto const &dir : {runtimeDir(), dataDir(), stateDir(), configDir(), cacheDir()}) {
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
