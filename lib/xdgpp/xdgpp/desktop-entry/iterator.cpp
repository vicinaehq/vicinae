#include "iterator.hpp"
#include "file.hpp"
#include <filesystem>
#include "../env/env.hpp"
#include <filesystem>
#include <system_error>
#include <set>

namespace fs = std::filesystem;

namespace xdgpp {

std::vector<xdgpp::DesktopFile> getAllDesktopEntries() { return getAllDesktopEntries(xdgpp::appDirs()); }

std::vector<xdgpp::DesktopFile> getAllDesktopEntries(const std::vector<std::filesystem::path> &paths) {
  std::vector<xdgpp::DesktopFile> list;
  std::set<std::string> seen;

  for (const auto &dir : paths) {
    std::error_code ec;
    for (const auto &entry : fs::recursive_directory_iterator(dir, ec)) {
      auto path = entry.path();
      auto filename = path.filename();
      fs::path id = DesktopFile::relativeId(path, dir);

      if (!filename.string().ends_with(".desktop")) continue;
      if (seen.contains(id)) continue;

      seen.insert(id);
      list.emplace_back(xdgpp::DesktopFile::fromFile(path, dir));
    }
  }

  return list;
}
}; // namespace xdgpp
