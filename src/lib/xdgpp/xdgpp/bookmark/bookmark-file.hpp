#pragma once
#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace xdgpp {

struct BookmarkApplication {
  std::string name;
  std::string exec;
  std::string modified;
  unsigned count = 0;
};

struct BookmarkIcon {
  std::string href;
  std::string type;
};

/**
 * Timestamps are ISO 8601 UTC strings and order the same way lexically as chronologically.
 */
struct Bookmark {
  std::string href;
  std::string title;
  std::string description;
  std::string added;
  std::string modified;
  std::string visited;
  std::optional<std::string> mimeType;
  std::optional<BookmarkIcon> icon;
  bool isPrivate = false;
  std::vector<std::string> groups;
  std::vector<BookmarkApplication> applications;

  /**
   * Most recent use of the bookmark. GTK refreshes `modified` on every use and only sets `visited`
   * when the item is first added, so `modified` is authoritative and the others are fallbacks.
   */
  const std::string &lastUsed() const;
};

/**
 * XBEL document following the desktop bookmark specification, as used for the shared
 * `recently-used.xbel` registry. Everything the specification defines is modeled, so a
 * document written by GTK or KDE survives a load and save round trip.
 */
class BookmarkFile {
public:
  /**
   * A missing file yields an empty document; a malformed one yields the parser error.
   */
  static std::expected<BookmarkFile, std::string> fromFile(const std::filesystem::path &path);
  static std::expected<BookmarkFile, std::string> fromData(std::string_view data);

  const std::vector<Bookmark> &bookmarks() const { return m_bookmarks; }
  const Bookmark *bookmark(std::string_view href) const;

  /**
   * Registers a use of `href` by the given application, creating the bookmark if needed and
   * refreshing its timestamps, like g_bookmark_file_add_application.
   */
  void addApplication(std::string_view href, std::string_view name, std::string_view exec);
  void setMimeType(std::string_view href, std::string_view mime);
  bool remove(std::string_view href);

  std::string toString() const;

  /**
   * Atomically replaces `path`, readable by the owner only like GTK does.
   */
  bool save(const std::filesystem::path &path) const;

private:
  Bookmark *find(std::string_view href);
  Bookmark &findOrCreate(std::string_view href, std::string_view timestamp);

  std::vector<Bookmark> m_bookmarks;
};

/**
 * `$XDG_DATA_HOME/recently-used.xbel`
 */
std::filesystem::path recentlyUsedFilePath();

} // namespace xdgpp
