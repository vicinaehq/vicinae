#pragma once
#include <cstdint>
#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace shortcut {

struct SerializedShortcut {
  std::string id;
  std::string name;
  std::string icon;
  std::string url;
  std::string app;
  int openCount = 0;
  std::uint64_t createdAt;
  std::uint64_t updatedAt;
  std::optional<std::uint64_t> lastUsedAt;
};

} // namespace shortcut

class ShortcutDatabase {
public:
  static constexpr size_t MAX_SHORTCUTS = 10000;

  ShortcutDatabase(const std::filesystem::path &path);

  std::expected<shortcut::SerializedShortcut, std::string>
  addShortcut(std::string_view name, std::string_view icon, std::string_view url, std::string_view app);
  std::expected<void, std::string> updateShortcut(std::string_view id, std::string_view name,
                                                  std::string_view icon, std::string_view url,
                                                  std::string_view app);
  std::expected<shortcut::SerializedShortcut, std::string> removeShortcut(std::string_view id);
  std::expected<void, std::string> registerVisit(std::string_view id);

  const std::vector<shortcut::SerializedShortcut> &shortcuts() const;
  shortcut::SerializedShortcut *findById(std::string_view id);

  std::expected<void, std::string> setShortcuts(std::span<shortcut::SerializedShortcut> shortcuts);
  void reload();

private:
  std::expected<std::vector<shortcut::SerializedShortcut>, std::string> loadShortcuts();

private:
  std::string m_buf;
  std::filesystem::path m_path;
  std::vector<shortcut::SerializedShortcut> m_shortcuts;
};
