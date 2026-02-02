#pragma once
#include "entry.hpp"

namespace xdgpp {
/**
 * A DesktopEntry that was obtained from the filesystem.
 * This one always has a valid `path` and `id` fields, unlike a plain desktop entry
 * that may have been constructed for raw data directly.
 */
class DesktopFile : public DesktopEntry {

public:
  static DesktopFile fromFile(const std::filesystem::path &file,
                              const std::optional<std::filesystem::path> &appDir);

  /**
   * Look for the desktop entry with that ID.
   * .desktop suffix is optional
   * IMPORTANT: currently doesn't handle nested desktop entries.
   */
  static std::optional<DesktopFile> fromId(std::string_view id,
                                           const std::vector<std::filesystem::path> &searchPaths);
  static std::optional<DesktopFile> fromId(std::string_view id);

  static std::string relativeId(const std::filesystem::path &file, const std::filesystem::path &appDir);

  const std::filesystem::path &path() const;
  std::string_view id() const;

private:
  DesktopFile(const std::string &id, const std::filesystem::path &path, const DesktopEntry &entry);

  std::filesystem::path m_path;
  std::string m_id;
};
}; // namespace xdgpp
