#include "env.hpp"
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <ranges>
#include <set>

namespace fs = std::filesystem;

static fs::path homeDir() {
  if (auto home = getenv("HOME")) { return home; }

  return {};
}

static std::string &toLowerCase(std::string &s) {
  std::ranges::transform(s, s.begin(), [](char c) { return std::tolower(c); });
  return s;
}

template <typename T> static std::vector<T> parseDirs(std::string_view data) {
  std::set<fs::path> seen;
  std::vector<T> paths;
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

std::vector<std::string> xdgpp::currentDesktop() {
  if (auto v = getenv("XDG_CURRENT_DESKTOP")) return parseDirs<std::string>(v);
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
  if (!xdd) { return {"/usr/local/share", "/usr/share"}; };

  return parseDirs<fs::path>(xdd);
}

std::vector<fs::path> xdgpp::commonDataDirs() {
  std::vector<fs::path> paths;
  auto dirs = xdgpp::dataDirs();
  auto home = dataHome();

  paths.reserve(dirs.size() + 1);
  paths.emplace_back(home);

  for (const auto &dir : dirs) {
    if (dir != home) { paths.emplace_back(dir); }
  }

  return paths;
}

std::vector<fs::path> xdgpp::appDirs() {
  std::vector<fs::path> paths;
  auto dirs = xdgpp::dataDirs();
  auto home = dataHome();

  paths.reserve(dirs.size() + 1);
  paths.emplace_back(home / "applications");

  for (const auto &dir : dirs) {
    if (dir != home) { paths.emplace_back(dir / "applications"); }
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

  return parseDirs<fs::path>(xcd);
}

std::optional<fs::path> xdgpp::runtimeDir() {
  if (auto v = getenv("XDG_RUNTIME_DIR")) return v;
  return {};
}

namespace {
std::vector<fs::path> getMimeLikeConfigPaths(std::string_view fileName) {
  std::vector<std::string> desktops = xdgpp::currentDesktop();
  std::ranges::transform(desktops, desktops.begin(), toLowerCase);

  std::vector<fs::path> paths;
  auto configDirs = xdgpp::configDirs();
  auto dataDirs = xdgpp::dataDirs();

  paths.reserve(configDirs.size() + dataDirs.size() + desktops.size() * 4);

  for (const auto &desktop : desktops) {
    const std::string desktopFileName = std::string(desktop).append("-").append(fileName);
    paths.emplace_back(xdgpp::configHome() / desktopFileName);
  }

  paths.emplace_back(xdgpp::configHome() / fileName);

  for (const auto &dir : configDirs) {
    for (const auto &desktop : desktops) {
      const std::string desktopFileName = std::string(desktop).append("-").append(fileName);
      paths.emplace_back(dir / desktopFileName);
    }

    paths.emplace_back(dir / fileName);
  }

  for (const auto &desktop : desktops) {
    const std::string desktopFileName = std::string(desktop).append("-").append(fileName);
    paths.emplace_back(xdgpp::dataHome() / "applications" / desktopFileName);
  }

  paths.emplace_back(xdgpp::dataHome() / "applications" / fileName);

  for (const auto &dir : dataDirs) {
    for (const auto &desktop : desktops) {
      const std::string desktopFileName = std::string(desktop).append("-").append(fileName);
      paths.emplace_back(dir / "applications" / desktopFileName);
    }

    paths.emplace_back(dir / "applications" / fileName);
  }

  return paths;
}
} // namespace

std::vector<fs::path> xdgpp::mimeAppsListPaths() { return getMimeLikeConfigPaths("mimeapps.list"); }

std::vector<fs::path> xdgpp::xdgTerminalsListPaths() {
  std::vector<std::string> desktops = xdgpp::currentDesktop();
  std::ranges::transform(desktops, desktops.begin(), toLowerCase);

  std::vector<fs::path> paths;
  auto cfgDirs = xdgpp::configDirs();
  auto dtDirs = xdgpp::dataDirs();

  paths.reserve((cfgDirs.size() + 1 + dtDirs.size()) * (desktops.size() + 1));

  // main config sequence: ${XDG_CONFIG_HOME} then ${XDG_CONFIG_DIRS}
  for (const auto &desktop : desktops) {
    paths.emplace_back(xdgpp::configHome() / (desktop + "-xdg-terminals.list"));
  }
  paths.emplace_back(xdgpp::configHome() / "xdg-terminals.list");

  for (const auto &dir : cfgDirs) {
    for (const auto &desktop : desktops) {
      paths.emplace_back(dir / (desktop + "-xdg-terminals.list"));
    }
    paths.emplace_back(dir / "xdg-terminals.list");
  }

  // upstream/distribution fallbacks: xdg-terminal-exec/ subdirs in ${XDG_DATA_DIRS} only
  for (const auto &dir : dtDirs) {
    for (const auto &desktop : desktops) {
      paths.emplace_back(dir / "xdg-terminal-exec" / (desktop + "-xdg-terminals.list"));
    }
    paths.emplace_back(dir / "xdg-terminal-exec" / "xdg-terminals.list");
  }

  return paths;
}
