#include <catch2/catch_test_macros.hpp>

#include "services/browser-extension-service.hpp"

TEST_CASE("browser metadata updates preserve tabs and refresh their labels") {
  BrowserExtensionService service;
  int changed = 0;
  QObject::connect(&service, &BrowserExtensionService::tabsChanged, &service, [&changed]() { ++changed; });

  service.registerBrowser({.id = "browser",
                           .name = "Chromium",
                           .engine = "chromium",
                           .version = "1",
                           .permissions = std::vector<std::string>{"bookmarks"}});
  service.setTabs("browser", {{.id = 7, .title = "Example", .url = "https://example.com"}});
  REQUIRE(service.tabs().size() == 1);
  CHECK(service.tabs().front().browserLabel == "Chromium");
  service.fetchBookmarks();
  REQUIRE(service.isFetchingBookmarks());

  service.registerBrowser({.id = "browser",
                           .name = "Brave · Work",
                           .engine = "chromium",
                           .version = "2",
                           .permissions = std::vector<std::string>{}});
  REQUIRE(service.tabs().size() == 1);
  CHECK(service.tabs().front().browserLabel == "Brave · Work");
  CHECK(service.tabs().front().id == 7);
  CHECK(service.browsers().front().version == "2");
  CHECK_FALSE(service.browsers().front().hasPermission("bookmarks"));
  CHECK_FALSE(service.isFetchingBookmarks());
  CHECK(changed == 3);
}

TEST_CASE("bookmark folders stay scoped to browser and disappear on permission revocation") {
  BrowserExtensionService service;
  service.registerBrowser({.id = "one", .permissions = std::vector<std::string>{"bookmarks"}});
  service.registerBrowser({.id = "two", .permissions = std::vector<std::string>{"bookmarks"}});
  service.setTabs("one", {{.id = 7, .url = "https://example.com"}});
  int requestId = -1;
  QObject::connect(&service, &BrowserExtensionService::bookmarksRequested, &service,
                   [&](const std::string &id, int request) {
                     CHECK(id == "one");
                     requestId = request;
                   });
  service.fetchBookmarks("one");
  REQUIRE(requestId >= 0);
  REQUIRE(service.setBookmarks("one", {.requestId = requestId,
                                       .bookmarks = {},
                                       .folders = std::vector<ipc_gen::BrowserBookmarkFolderInfo>{
                                           {.id = "folder", .path = {"Bookmarks bar", "Empty"}}}}));
  REQUIRE(service.browsers().front().folders.size() == 1);
  CHECK(service.browsers().front().folders.front().id == "folder");
  CHECK(service.browsers().back().folders.empty());

  std::optional<std::string> selectedFolder;
  QObject::connect(&service, &BrowserExtensionService::tabActionRequested, &service,
                   [&](const std::string &, int, BrowserExtensionService::TabAction, const std::string &,
                       const std::string &,
                       const std::optional<std::string> &folder) { selectedFolder = folder; });
  REQUIRE(service.bookmarkTab("one", 7, {}, {}, "folder"));
  CHECK(selectedFolder == "folder");
  service.setPermissions("one", {});
  CHECK(service.browsers().front().folders.empty());
}

TEST_CASE("old extension user agents render short browser labels") {
  BrowserExtensionService service;
  service.registerBrowser({.id = "firefox",
                           .name = "Mozilla/5.0 (X11; Linux x86_64; rv:139.0) Gecko/20100101 Firefox/139.0",
                           .engine = "chromium"});
  service.setTabs("firefox", {{.id = 1}});
  REQUIRE(service.tabs().size() == 1);
  CHECK(service.tabs().front().browserLabel == "Firefox");

  service.registerBrowser({.id = "chromium", .name = "Mozilla/5.0 Chrome/139.0 Safari/537.36"});
  service.setTabs("chromium", {{.id = 2}});
  CHECK(service.tabs().back().browserLabel == "Chromium");
  REQUIRE(service.tabs("firefox").size() == 1);
  CHECK(service.tabs("firefox").front().id == 1);
  CHECK(service.tabs("chromium").front().id == 2);
  CHECK(service.tabs("missing").empty());
}

TEST_CASE("same-named browsers show short IDs on tab rows") {
  BrowserExtensionService service;
  service.registerBrowser({.id = "abcdef01-first", .name = "Firefox"});
  service.registerBrowser({.id = "12345678-second", .name = "Firefox"});
  service.setTabs("abcdef01-first", {{.id = 1}});
  service.setTabs("12345678-second", {{.id = 2}});

  const auto filtered = service.tabs("abcdef01-first");
  REQUIRE(filtered.size() == 1);
  CHECK(filtered.front().displayBrowserLabel(QStringLiteral("Browser")) ==
        QStringLiteral("Firefox (abcdef01)"));
  CHECK(service.tabs("12345678-second").front().browserShortId == "12345678");

  service.unregisterBrowser("12345678-second");
  CHECK(service.tabs().front().displayBrowserLabel(QStringLiteral("Browser")) == QStringLiteral("Firefox"));
}
