#include "file.hpp"
#include "entry.hpp"
#include "../env/env.hpp"
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

namespace xdgpp {

DesktopFile DesktopFile::fromFile(const std::filesystem::path &file,
                                  const std::optional<std::filesystem::path> &appDir) {
  auto id = appDir ? DesktopFile::relativeId(file, appDir.value()) : file.filename().string();

  return DesktopFile(id, file, DesktopEntry::fromFile(file));
}

std::string DesktopFile::relativeId(const std::filesystem::path &file, const std::filesystem::path &appDir) {
  std::string id = file.lexically_relative(appDir);
  auto normalize = [](char c) { return c == '/' ? '.' : c; };
  std::transform(id.begin(), id.end(), id.begin(), normalize);

  return id;
}

const std::filesystem::path &DesktopFile::path() const { return m_path; }

std::string_view DesktopFile::id() const { return m_id; }

std::optional<DesktopFile> DesktopFile::fromId(std::string_view id) { return fromId(id, appDirs()); }

std::optional<DesktopFile> DesktopFile::fromId(std::string_view id, const std::vector<fs::path> &paths) {
  std::error_code ec;

  for (const auto &dir : paths) {
    fs::path full = dir / id;
    fs::path alt = dir / (std::string(id) + ".desktop");

    for (const auto &path : std::vector<fs::path>{full, alt}) {
      if (fs::is_regular_file(path, ec)) { return DesktopFile::fromFile(path, dir); }
    }
  }

  return {};
}

DesktopFile::DesktopFile(const std::string &id, const std::filesystem::path &path, const DesktopEntry &entry)
    : DesktopEntry(entry), m_path(path), m_id(id) {}
}; // namespace xdgpp
