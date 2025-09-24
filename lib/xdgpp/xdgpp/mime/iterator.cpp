#include "iterator.hpp"
#include "../env/env.hpp"

namespace xdgpp {
std::vector<MimeAppsListFile> getAllMimeAppsLists() {
  std::vector<MimeAppsListFile> files;
  std::error_code ec;
  auto paths = xdgpp::mimeAppsListPaths();

  files.reserve(paths.size());
  for (const auto &path : paths) {
    files.emplace_back(MimeAppsListFile::fromFile(path));
  }

  return files;
}
}; // namespace xdgpp
