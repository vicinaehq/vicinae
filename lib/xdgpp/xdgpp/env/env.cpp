#include "env.hpp"
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <set>

namespace fs = std::filesystem;

static fs::path homeDir() {
  if (auto home = getenv("HOME")) { return home; }

  return {};
}

static std::string &toLowerCase(std::string &s) {
  std::transform(s.begin(), s.end(), s.begin(), [](char c) { return std::tolower(c); });
  return s;
}

static std::vector<fs::path> parseDirs(std::string_view data) {
  std::set<fs::path> seen;
  std::vector<fs::path> paths;
  std::string current;

  for (const auto &c : data) {
    if (c == ':' && !current.empty()) {
      if (!seen.contains(current)) {
        seen.insert(current);
        paths.emplace_back(current);
      }
      current.clear();
    } else {
      current += c;
    }
  }

  if (!current.empty() && !seen.contains(current)) { paths.emplace_back(current); }

  return paths;
}

std::optional<std::string> xdgpp::currentDesktop() {
  if (auto v = getenv("XDG_CURRENT_DESKTOP")) return v;
  return {};
}

fs::path xdgpp::dataHome() {
  if (auto v = getenv("XDG_DATA_HOME")) return v;
  return homeDir() / ".local" / "share";
}

fs::path xdgpp::cacheHome() {
  if (auto v = getenv("XDG_CACHE_HOME")) return v;
  return homeDir() / ".cache";
}

fs::path xdgpp::stateHome() {
  if (auto v = getenv("XDG_STATE_HOME")) return v;
  return homeDir() / ".local" / "state";
}

std::vector<fs::path> xdgpp::dataDirs() {
  const char *xdd = getenv("XDG_DATA_DIRS");

  // the spec doesn't require to add the local dir as a default but we do
  if (!xdd) { return {homeDir() / ".local" / "share", "/usr/local/share", "/usr/share"}; };

  return parseDirs(xdd);
}

std::vector<fs::path> xdgpp::appDirs() {
  std::vector<fs::path> paths;
  auto dirs = xdgpp::dataDirs();

  paths.reserve(dirs.size());

  for (const auto &dir : dirs) {
    paths.emplace_back(dir / "applications");
  }

  return paths;
}

fs::path xdgpp::configHome() {
  if (auto v = getenv("XDG_CONFIG_HOME")) return v;
  return homeDir() / ".config";
}

std::vector<fs::path> xdgpp::configDirs() {
  const char *xcd = getenv("XDG_CONFIG_DIRS");

  if (!xcd) { return {"/etc/xdg"}; };

  return parseDirs(xcd);
}

std::optional<std::filesystem::path> xdgpp::runtimeDir() {
  if (auto v = getenv("XDG_RUNTIME_DIR")) return v;
  return {};
}

std::vector<std::filesystem::path> xdgpp::mimeAppsListPaths() {
  std::string desktop = xdgpp::currentDesktop().value_or("unknown");

  toLowerCase(desktop);
  std::vector<fs::path> paths;
  std::string fileName = "mimeapps.list";
  std::string desktopFileName = desktop + "-" + fileName;
  auto configDirs = xdgpp::configDirs();
  auto dataDirs = xdgpp::dataDirs();

  paths.reserve(configDirs.size() + dataDirs.size() + 4);
  paths.emplace_back(xdgpp::configHome() / desktopFileName);
  paths.emplace_back(xdgpp::configHome() / fileName);

  for (const auto &dir : configDirs) {
    paths.emplace_back(dir / desktopFileName);
    paths.emplace_back(dir / fileName);
  }

  paths.emplace_back(xdgpp::dataHome() / "applications" / desktopFileName);
  paths.emplace_back(xdgpp::dataHome() / "applications" / fileName);

  for (const auto &dir : dataDirs) {
    paths.emplace_back(dir / "applications" / desktopFileName);
    paths.emplace_back(dir / "applications" / fileName);
  }

  return paths;
}
