#include "vicinae.hpp"
#include "utils/utils.hpp"
#include <qprocess.h>
#include <QProcessEnvironment>
#include <ranges>
#include <set>

namespace fs = std::filesystem;

fs::path Omnicast::runtimeDir() {
  fs::path osRundir(QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation).toStdString());

  return osRundir / "vicinae";
}

fs::path Omnicast::dataDir() { return homeDir() / ".local" / "share" / "vicinae"; }

fs::path Omnicast::configDir() { return homeDir() / ".config" / "vicinae"; }

fs::path Omnicast::commandSocketPath() { return runtimeDir() / "vicinae.sock"; }
fs::path Omnicast::pidFile() { return runtimeDir() / "vicinae.pid"; }

std::vector<fs::path> Omnicast::systemPaths() {
  const char *path = getenv("PATH");

  if (!path) return {};

  std::set<fs::path> seen;
  std::vector<fs::path> paths;

  for (const auto &part : std::views::split(std::string_view(path), std::string_view(":"))) {
    fs::path path = std::string_view(part.begin(), part.end());

    if (seen.contains(path)) continue;

    seen.insert(path);
    paths.emplace_back(path);
  }

  return paths;
}
