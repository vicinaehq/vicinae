#pragma once
#include "mime-apps-list.hpp"
#include <filesystem>
#include "../utils/utils.hpp"

namespace fs = std::filesystem;

namespace xdgpp {

/**
 * a MimeAppsList explicitly tied to a file.
 * If you want to parse a mimeapps.list file from data directly, use `MimeAppsList`.
 */
class MimeAppsListFile : public MimeAppsList {
public:
  static MimeAppsListFile fromFile(const fs::path &path) {
    return MimeAppsListFile(path, MimeAppsList::fromData(slurp(path)));
  }

  const std::filesystem::path &path() const { return m_path; }

private:
  MimeAppsListFile(const fs::path &path, const MimeAppsList &list) : MimeAppsList(list), m_path(path) {}

  fs::path m_path;
};

}; // namespace xdgpp
