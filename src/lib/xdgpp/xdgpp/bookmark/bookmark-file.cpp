#include "bookmark-file.hpp"
#include "../env/env.hpp"
#include "../uri/file-uri.hpp"
#include "../utils/utils.hpp"
#include <algorithm>
#include <chrono>
#include <format>
#include <fstream>
#include <pugixml.hpp>
#include <sstream>
#include <system_error>

namespace fs = std::filesystem;

namespace xdgpp {

namespace {

constexpr auto RECENTLY_USED_FILE_NAME = "recently-used.xbel";
constexpr auto BOOKMARK_NS = "http://www.freedesktop.org/standards/desktop-bookmarks";
constexpr auto MIME_NS = "http://www.freedesktop.org/standards/shared-mime-info";
constexpr auto METADATA_OWNER = "http://freedesktop.org";

std::string now() {
  auto const time = std::chrono::floor<std::chrono::microseconds>(std::chrono::system_clock::now());
  return std::format("{:%FT%TZ}", time);
}

void setAttribute(pugi::xml_node node, const char *name, std::string_view value) {
  node.append_attribute(name).set_value(std::string(value).c_str());
}

void setText(pugi::xml_node parent, const char *name, std::string_view value) {
  if (value.empty()) return;
  parent.append_child(name).text().set(std::string(value).c_str());
}

Bookmark parseBookmark(pugi::xml_node node) {
  Bookmark bookmark{.href = node.attribute("href").value(),
                    .title = node.child("title").text().get(),
                    .description = node.child("desc").text().get(),
                    .added = node.attribute("added").value(),
                    .modified = node.attribute("modified").value(),
                    .visited = node.attribute("visited").value()};

  auto meta = node.child("info").child("metadata");
  bookmark.isPrivate = meta.child("bookmark:private");

  if (auto mime = meta.child("mime:mime-type").attribute("type")) bookmark.mimeType = mime.value();

  if (auto icon = meta.child("bookmark:icon")) {
    bookmark.icon =
        BookmarkIcon{.href = icon.attribute("href").value(), .type = icon.attribute("type").value()};
  }

  for (auto group : meta.child("bookmark:groups").children("bookmark:group")) {
    bookmark.groups.emplace_back(group.text().get());
  }

  for (auto app : meta.child("bookmark:applications").children("bookmark:application")) {
    bookmark.applications.emplace_back(BookmarkApplication{.name = app.attribute("name").value(),
                                                           .exec = app.attribute("exec").value(),
                                                           .modified = app.attribute("modified").value(),
                                                           .count = app.attribute("count").as_uint()});
  }

  return bookmark;
}

void writeBookmark(pugi::xml_node root, const Bookmark &bookmark) {
  auto node = root.append_child("bookmark");
  setAttribute(node, "href", bookmark.href);
  setAttribute(node, "added", bookmark.added);
  setAttribute(node, "modified", bookmark.modified);
  setAttribute(node, "visited", bookmark.visited);
  setText(node, "title", bookmark.title);
  setText(node, "desc", bookmark.description);

  auto meta = node.append_child("info").append_child("metadata");
  setAttribute(meta, "owner", METADATA_OWNER);

  if (bookmark.mimeType) setAttribute(meta.append_child("mime:mime-type"), "type", *bookmark.mimeType);

  if (!bookmark.groups.empty()) {
    auto groups = meta.append_child("bookmark:groups");
    for (auto const &group : bookmark.groups) {
      setText(groups, "bookmark:group", group);
    }
  }

  if (!bookmark.applications.empty()) {
    auto apps = meta.append_child("bookmark:applications");
    for (auto const &app : bookmark.applications) {
      auto appNode = apps.append_child("bookmark:application");
      setAttribute(appNode, "name", app.name);
      setAttribute(appNode, "exec", app.exec);
      setAttribute(appNode, "modified", app.modified);
      appNode.append_attribute("count").set_value(app.count);
    }
  }

  if (bookmark.isPrivate) meta.append_child("bookmark:private");

  if (bookmark.icon) {
    auto icon = meta.append_child("bookmark:icon");
    setAttribute(icon, "href", bookmark.icon->href);
    setAttribute(icon, "type", bookmark.icon->type);
  }
}

} // namespace

const std::string &Bookmark::lastUsed() const {
  if (!modified.empty()) return modified;
  if (!visited.empty()) return visited;
  return added;
}

std::expected<BookmarkFile, std::string> BookmarkFile::fromFile(const fs::path &path) {
  std::error_code ec;
  if (!fs::exists(path, ec)) return BookmarkFile{};
  return fromData(slurp(path));
}

std::expected<BookmarkFile, std::string> BookmarkFile::fromData(std::string_view data) {
  pugi::xml_document doc;
  auto result = doc.load_buffer(data.data(), data.size());
  if (!result) return std::unexpected(result.description());

  BookmarkFile file;
  for (auto node : doc.child("xbel").children("bookmark")) {
    file.m_bookmarks.emplace_back(parseBookmark(node));
  }

  return file;
}

Bookmark *BookmarkFile::find(std::string_view href) {
  auto byHref = std::ranges::find(m_bookmarks, href, &Bookmark::href);
  if (byHref != m_bookmarks.end()) return &*byHref;

  auto path = fromFileUri(href);
  if (!path) return nullptr;

  auto byPath =
      std::ranges::find_if(m_bookmarks, [&](const Bookmark &b) { return fromFileUri(b.href) == path; });
  return byPath != m_bookmarks.end() ? &*byPath : nullptr;
}

const Bookmark *BookmarkFile::bookmark(std::string_view href) const {
  return const_cast<BookmarkFile *>(this)->find(href);
}

Bookmark &BookmarkFile::findOrCreate(std::string_view href, std::string_view timestamp) {
  if (auto bookmark = find(href)) return *bookmark;

  return m_bookmarks.emplace_back(Bookmark{.href = std::string(href), .added = std::string(timestamp)});
}

void BookmarkFile::addApplication(std::string_view href, std::string_view name, std::string_view exec) {
  auto const timestamp = now();
  auto &bookmark = findOrCreate(href, timestamp);

  bookmark.modified = timestamp;
  bookmark.visited = timestamp;

  auto app = std::ranges::find(bookmark.applications, name, &BookmarkApplication::name);

  if (app == bookmark.applications.end()) {
    bookmark.applications.emplace_back(BookmarkApplication{
        .name = std::string(name), .exec = std::string(exec), .modified = timestamp, .count = 1});
    return;
  }

  app->exec = exec;
  app->modified = timestamp;
  ++app->count;
}

void BookmarkFile::setMimeType(std::string_view href, std::string_view mime) {
  if (auto bookmark = find(href)) bookmark->mimeType = std::string(mime);
}

bool BookmarkFile::remove(std::string_view href) {
  auto bookmark = find(href);
  if (!bookmark) return false;

  m_bookmarks.erase(m_bookmarks.begin() + (bookmark - m_bookmarks.data()));
  return true;
}

std::string BookmarkFile::toString() const {
  pugi::xml_document doc;

  auto decl = doc.append_child(pugi::node_declaration);
  decl.append_attribute("version").set_value("1.0");
  decl.append_attribute("encoding").set_value("UTF-8");

  auto root = doc.append_child("xbel");
  root.append_attribute("version").set_value("1.0");
  root.append_attribute("xmlns:bookmark").set_value(BOOKMARK_NS);
  root.append_attribute("xmlns:mime").set_value(MIME_NS);

  for (auto const &bookmark : m_bookmarks) {
    writeBookmark(root, bookmark);
  }

  std::ostringstream oss;
  doc.save(oss, "  ");
  return oss.str();
}

bool BookmarkFile::save(const fs::path &path) const {
  std::error_code ec;
  fs::create_directories(path.parent_path(), ec);

  auto tmp = path;
  tmp += ".tmp";

  {
    std::ofstream out(tmp, std::ios::trunc);
    if (!out) return false;
    out << toString();
    if (!out) return false;
  }

  fs::permissions(tmp, fs::perms::owner_read | fs::perms::owner_write, ec);
  fs::rename(tmp, path, ec);
  if (ec) fs::remove(tmp);
  return !ec;
}

fs::path recentlyUsedFilePath() { return dataHome() / RECENTLY_USED_FILE_NAME; }

} // namespace xdgpp
