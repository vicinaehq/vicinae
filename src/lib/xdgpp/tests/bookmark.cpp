#include <catch2/catch_test_macros.hpp>
#include <xdgpp/xdgpp.hpp>

namespace {

constexpr auto SAMPLE = R"(<?xml version="1.0" encoding="UTF-8"?>
<xbel version="1.0"
      xmlns:bookmark="http://www.freedesktop.org/standards/desktop-bookmarks"
      xmlns:mime="http://www.freedesktop.org/standards/shared-mime-info"
>
  <bookmark href="file:///tmp/test.mp4" added="2025-11-14T21:04:28.221405Z" modified="2026-08-07T08:42:13.260987Z" visited="2025-11-14T21:04:28.221406Z">
    <info>
      <metadata owner="http://freedesktop.org">
        <mime:mime-type type="video/mp4"/>
        <bookmark:applications>
          <bookmark:application name="xdg-desktop-portal-gtk" exec="&apos;xdg-desktop-portal-gtk %u&apos;" modified="2026-08-07T08:42:13.260986Z" count="24"/>
          <bookmark:application name="org.gnome.Nautilus" exec="&apos;mpv -- %U&apos;" modified="2026-04-02T12:11:18.113646Z" count="1"/>
        </bookmark:applications>
      </metadata>
    </info>
  </bookmark>
  <bookmark href="file:///home/user/secret.txt" added="2026-01-01T00:00:00Z" modified="2026-01-01T00:00:00Z" visited="2026-01-01T00:00:00Z">
    <title>Secret</title>
    <desc>Do not share</desc>
    <info>
      <metadata owner="http://freedesktop.org">
        <bookmark:groups>
          <bookmark:group>Work</bookmark:group>
          <bookmark:group>Notes</bookmark:group>
        </bookmark:groups>
        <bookmark:private/>
        <bookmark:icon href="file:///home/user/icon.png" type="image/png"/>
      </metadata>
    </info>
  </bookmark>
</xbel>
)";

} // namespace

TEST_CASE("parses recently-used bookmarks", XDGPP_GROUP) {
  auto file = xdgpp::BookmarkFile::fromData(SAMPLE);
  REQUIRE(file);

  auto bookmarks = file->bookmarks();
  REQUIRE(bookmarks.size() == 2);

  auto const &video = bookmarks[0];
  REQUIRE(video.href == "file:///tmp/test.mp4");
  REQUIRE(video.added == "2025-11-14T21:04:28.221405Z");
  REQUIRE(video.modified == "2026-08-07T08:42:13.260987Z");
  REQUIRE(video.mimeType == "video/mp4");
  REQUIRE_FALSE(video.isPrivate);
  REQUIRE(video.applications.size() == 2);
  REQUIRE(video.applications[0].name == "xdg-desktop-portal-gtk");
  REQUIRE(video.applications[0].exec == "'xdg-desktop-portal-gtk %u'");
  REQUIRE(video.applications[0].count == 24);

  auto const &secret = bookmarks[1];
  REQUIRE(secret.isPrivate);
  REQUIRE_FALSE(secret.mimeType);
  REQUIRE(secret.title == "Secret");
  REQUIRE(secret.description == "Do not share");
  REQUIRE(secret.groups == std::vector<std::string>{"Work", "Notes"});
  REQUIRE(secret.icon);
  REQUIRE(secret.icon->href == "file:///home/user/icon.png");
  REQUIRE(secret.icon->type == "image/png");
}

TEST_CASE("round trips every modeled field", XDGPP_GROUP) {
  auto file = xdgpp::BookmarkFile::fromData(SAMPLE);
  REQUIRE(file);

  auto reparsed = xdgpp::BookmarkFile::fromData(file->toString());
  REQUIRE(reparsed);

  auto const &before = file->bookmarks();
  auto const &after = reparsed->bookmarks();
  REQUIRE(after.size() == before.size());

  for (size_t i = 0; i < before.size(); ++i) {
    REQUIRE(after[i].href == before[i].href);
    REQUIRE(after[i].title == before[i].title);
    REQUIRE(after[i].description == before[i].description);
    REQUIRE(after[i].added == before[i].added);
    REQUIRE(after[i].modified == before[i].modified);
    REQUIRE(after[i].visited == before[i].visited);
    REQUIRE(after[i].mimeType == before[i].mimeType);
    REQUIRE(after[i].isPrivate == before[i].isPrivate);
    REQUIRE(after[i].groups == before[i].groups);
    REQUIRE(after[i].icon.has_value() == before[i].icon.has_value());
    REQUIRE(after[i].applications.size() == before[i].applications.size());
    for (size_t j = 0; j < before[i].applications.size(); ++j) {
      REQUIRE(after[i].applications[j].name == before[i].applications[j].name);
      REQUIRE(after[i].applications[j].exec == before[i].applications[j].exec);
      REQUIRE(after[i].applications[j].modified == before[i].applications[j].modified);
      REQUIRE(after[i].applications[j].count == before[i].applications[j].count);
    }
  }
}

TEST_CASE("last use falls back through the timestamps", XDGPP_GROUP) {
  xdgpp::Bookmark bookmark{.added = "2026-01-01T00:00:00Z"};
  REQUIRE(bookmark.lastUsed() == "2026-01-01T00:00:00Z");

  bookmark.visited = "2026-02-01T00:00:00Z";
  REQUIRE(bookmark.lastUsed() == "2026-02-01T00:00:00Z");

  bookmark.modified = "2026-03-01T00:00:00Z";
  REQUIRE(bookmark.lastUsed() == "2026-03-01T00:00:00Z");

  auto file = xdgpp::BookmarkFile::fromData(SAMPLE);
  REQUIRE(file);
  REQUIRE(file->bookmarks()[0].lastUsed() == "2026-08-07T08:42:13.260987Z");
}

TEST_CASE("rejects malformed documents", XDGPP_GROUP) {
  REQUIRE_FALSE(xdgpp::BookmarkFile::fromData("<xbel><bookmark></xbel>"));
}

TEST_CASE("adds an application to an existing bookmark", XDGPP_GROUP) {
  auto file = xdgpp::BookmarkFile::fromData(SAMPLE);
  REQUIRE(file);

  file->addApplication("file:///tmp/test.mp4", "vicinae", "'vicinae %u'");
  file->addApplication("file:///tmp/test.mp4", "vicinae", "'vicinae %u'");

  auto video = file->bookmark("file:///tmp/test.mp4");
  REQUIRE(video);
  REQUIRE(video->added == "2025-11-14T21:04:28.221405Z");
  REQUIRE(video->modified > "2026-08-07T08:42:13.260987Z");
  REQUIRE(video->visited == video->modified);
  REQUIRE(video->applications.size() == 3);
  REQUIRE(video->applications[2].name == "vicinae");
  REQUIRE(video->applications[2].count == 2);
  REQUIRE(video->applications[0].count == 24);
  REQUIRE(file->bookmarks().size() == 2);
}

TEST_CASE("creates bookmarks in an empty document", XDGPP_GROUP) {
  xdgpp::BookmarkFile file;

  file.addApplication("file:///home/user/notes.txt", "vicinae", "'vicinae %u'");
  file.setMimeType("file:///home/user/notes.txt", "text/plain");

  auto bookmark = file.bookmark("file:///home/user/notes.txt");
  REQUIRE(bookmark);
  REQUIRE(bookmark->added == bookmark->modified);
  REQUIRE(bookmark->mimeType == "text/plain");
  REQUIRE(bookmark->applications.size() == 1);
  REQUIRE(bookmark->applications[0].count == 1);

  auto text = file.toString();
  REQUIRE(text.starts_with("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
  REQUIRE(text.contains("xmlns:bookmark=\"http://www.freedesktop.org/standards/desktop-bookmarks\""));
  REQUIRE(text.contains("xmlns:mime=\"http://www.freedesktop.org/standards/shared-mime-info\""));
  REQUIRE(text.contains("<bookmark:application name=\"vicinae\" exec=\"'vicinae %u'\""));

  auto reparsed = xdgpp::BookmarkFile::fromData(text);
  REQUIRE(reparsed);
  REQUIRE(reparsed->bookmarks().size() == 1);
}

TEST_CASE("matches bookmarks by decoded path", XDGPP_GROUP) {
  xdgpp::BookmarkFile file;

  file.addApplication("file:///home/user/my%20file.txt", "a", "a");
  file.addApplication("file:///home/user/my%20File.txt", "b", "b");
  file.addApplication("file:///home/user/my file.txt", "c", "c");

  REQUIRE(file.bookmarks().size() == 2);
  REQUIRE(file.bookmark("file:///home/user/my%20file.txt")->applications.size() == 2);
}

TEST_CASE("removes bookmarks", XDGPP_GROUP) {
  auto file = xdgpp::BookmarkFile::fromData(SAMPLE);
  REQUIRE(file);
  REQUIRE(file->remove("file:///tmp/test.mp4"));
  REQUIRE_FALSE(file->remove("file:///tmp/test.mp4"));
  REQUIRE(file->bookmarks().size() == 1);
}

TEST_CASE("saves and reloads from disk", XDGPP_GROUP) {
  auto dir = std::filesystem::temp_directory_path() / "xdgpp-bookmark-test";
  std::filesystem::remove_all(dir);
  auto path = dir / "recently-used.xbel";

  auto missing = xdgpp::BookmarkFile::fromFile(path);
  REQUIRE(missing);
  REQUIRE(missing->bookmarks().empty());

  missing->addApplication("file:///tmp/a.txt", "vicinae", "'vicinae %u'");
  REQUIRE(missing->save(path));

  auto perms = std::filesystem::status(path).permissions();
  REQUIRE((perms & std::filesystem::perms::group_all) == std::filesystem::perms::none);
  REQUIRE((perms & std::filesystem::perms::others_all) == std::filesystem::perms::none);

  auto loaded = xdgpp::BookmarkFile::fromFile(path);
  REQUIRE(loaded);
  REQUIRE(loaded->bookmarks().size() == 1);
  REQUIRE(loaded->bookmarks()[0].href == "file:///tmp/a.txt");

  std::filesystem::remove_all(dir);
}
