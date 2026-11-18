#include <array>
#include <QCoreApplication>
#include <QPromise>
#include <catch2/catch_test_macros.hpp>

#include "services/browser-extension-service.hpp"

namespace {

class TestBrowserPeer : public AbstractBrowserPeer {
public:
  using Bookmarks = ipc_gen::Result<std::vector<ipc_gen::BrowserBookmarkInfo>>;
  using Folders = ipc_gen::Result<std::vector<ipc_gen::BrowserBookmarkFolderInfo>>;
  QPromise<Bookmarks::Type> bookmarksPromise;
  QPromise<Folders::Type> foldersPromise;
  std::optional<ipc_gen::BookmarkTabRequest> savedBookmark;
  std::optional<std::string> actionError;
  int bookmarksCalls = 0;
  int foldersCalls = 0;
  int pinnedTab = 0;
  bool pinnedValue = false;

  TestBrowserPeer() {
    bookmarksPromise.start();
    foldersPromise.start();
  }

  Future<void> focusTab(int) override { return ipc_gen::Result<void>::ok(); }
  Future<void> closeTab(int) override { return ipc_gen::Result<void>::ok(); }
  Future<void> setPinned(int tabId, bool pinned) override {
    pinnedTab = tabId;
    pinnedValue = pinned;
    return actionResult();
  }
  Future<void> setMuted(int, bool) override { return actionResult(); }
  Future<void> bookmarkTab(const ipc_gen::BookmarkTabRequest &req) override {
    savedBookmark = req;
    return actionResult();
  }
  Future<void> duplicateTab(int) override { return actionResult(); }
  Future<std::vector<ipc_gen::BrowserBookmarkInfo>> bookmarks() override {
    ++bookmarksCalls;
    return bookmarksPromise.future();
  }
  Future<std::vector<ipc_gen::BrowserBookmarkFolderInfo>> bookmarkFolders() override {
    ++foldersCalls;
    return foldersPromise.future();
  }
  Future<void> removeBookmark(const std::string &) override { return actionResult(); }
  Future<std::vector<ipc_gen::BrowserHistoryItemInfo>> searchHistory(const std::string &, int) override {
    return ipc_gen::Result<std::vector<ipc_gen::BrowserHistoryItemInfo>>::ok({});
  }
  Future<void> removeHistoryUrl(const std::string &) override { return actionResult(); }
  Future<void> clearHistory() override { return actionResult(); }
  Future<void> requestPermission(const std::string &) override { return actionResult(); }

private:
  Future<void> actionResult() {
    return actionError ? ipc_gen::Result<void>::fail(*actionError) : ipc_gen::Result<void>::ok();
  }
};

struct BrowserServiceTest {
  int argc = 1;
  std::string name = "browser-tests";
  std::array<char *, 2> argv{name.data(), nullptr};
  QCoreApplication app{argc, argv.data()};
  BrowserExtensionService service;
};

} // namespace

TEST_CASE_METHOD(BrowserServiceTest, "browser metadata updates preserve tabs and refresh their labels") {
  auto peer = std::make_shared<TestBrowserPeer>();
  int changed = 0;
  QObject::connect(&service, &BrowserExtensionService::tabsChanged, &service, [&changed]() { ++changed; });

  service.registerBrowser({.id = "browser",
                           .name = "Chromium",
                           .engine = "chromium",
                           .version = "1",
                           .permissions = std::vector<std::string>{"bookmarks"},
                           .peer = peer});
  service.setTabs("browser", {{.id = 7, .title = "Example", .url = "https://example.com"}});
  REQUIRE(service.tabs().size() == 1);
  CHECK(service.tabs().front().browserLabel == "Chromium");
  service.fetchBookmarks();
  REQUIRE(service.isFetchingBookmarks());

  service.registerBrowser({.id = "browser",
                           .name = "Brave · Work",
                           .engine = "chromium",
                           .version = "2",
                           .permissions = std::vector<std::string>{},
                           .peer = peer});
  REQUIRE(service.tabs().size() == 1);
  CHECK(service.tabs().front().browserLabel == "Brave · Work");
  CHECK(service.tabs().front().id == 7);
  CHECK(service.browsers().front().version == "2");
  CHECK_FALSE(service.browsers().front().hasPermission("bookmarks"));
  CHECK_FALSE(service.isFetchingBookmarks());
  CHECK(changed == 3);
}

TEST_CASE_METHOD(BrowserServiceTest,
                 "bookmark folders stay scoped to browser and disappear on permission revocation") {
  auto peer = std::make_shared<TestBrowserPeer>();
  auto other = std::make_shared<TestBrowserPeer>();
  service.registerBrowser({.id = "one", .permissions = std::vector<std::string>{"bookmarks"}, .peer = peer});
  service.registerBrowser({.id = "two", .permissions = std::vector<std::string>{"bookmarks"}, .peer = other});
  service.setTabs("one", {{.id = 7, .url = "https://example.com"}});
  service.fetchBookmarks("one");
  CHECK(peer->bookmarksCalls == 1);
  CHECK(peer->foldersCalls == 1);
  CHECK(other->bookmarksCalls == 0);
  CHECK(other->foldersCalls == 0);
  peer->bookmarksPromise.addResult(std::vector<ipc_gen::BrowserBookmarkInfo>{});
  peer->bookmarksPromise.finish();
  peer->foldersPromise.addResult(
      std::vector<ipc_gen::BrowserBookmarkFolderInfo>{{.id = "folder", .path = {"Bookmarks bar", "Empty"}}});
  peer->foldersPromise.finish();
  QCoreApplication::processEvents();
  REQUIRE(service.browsers().front().folders.size() == 1);
  CHECK(service.browsers().front().folders.front().id == "folder");
  CHECK(service.browsers().back().folders.empty());

  REQUIRE(service.bookmarkTab("one", 7, "Edited", "https://edited.example", "folder").result());
  REQUIRE(peer->savedBookmark);
  CHECK(peer->savedBookmark->folderId == "folder");
  CHECK(peer->savedBookmark->title == "Edited");
  CHECK(peer->savedBookmark->url == "https://edited.example");
  service.setPermissions("one", {});
  CHECK(service.browsers().front().folders.empty());
}

TEST_CASE_METHOD(BrowserServiceTest, "revoked bookmark permission ignores pending folder responses") {
  auto peer = std::make_shared<TestBrowserPeer>();
  service.registerBrowser({.id = "one", .permissions = std::vector<std::string>{"bookmarks"}, .peer = peer});
  service.fetchBookmarks("one");
  service.setPermissions("one", {});
  peer->foldersPromise.addResult(std::vector<ipc_gen::BrowserBookmarkFolderInfo>{{.id = "stale"}});
  peer->foldersPromise.finish();
  peer->bookmarksPromise.addResult(std::vector<ipc_gen::BrowserBookmarkInfo>{});
  peer->bookmarksPromise.finish();
  QCoreApplication::processEvents();
  CHECK(service.browsers().front().folders.empty());
  CHECK_FALSE(service.browsers().front().bookmarks);
}

TEST_CASE_METHOD(BrowserServiceTest, "tab actions validate targets and propagate browser failures") {
  auto peer = std::make_shared<TestBrowserPeer>();
  service.registerBrowser({.id = "one", .peer = peer});
  service.registerBrowser({.id = "outdated"});
  service.setTabs("one", {{.id = 7, .title = "Example", .url = "https://example.com"}});
  CHECK(service.setPinned("missing", 7, true).result().error() == "No such browser");
  CHECK(service.setPinned("outdated", 7, true).result().error() ==
        BrowserExtensionService::OUTDATED_EXTENSION_ERROR);
  CHECK(service.setPinned("one", 99, true).result().error() == "No such tab");
  REQUIRE(service.setPinned("one", 7, true).result());
  CHECK(peer->pinnedTab == 7);
  CHECK(peer->pinnedValue);
  CHECK(service.bookmarkTab("one", 7, {}, "invalid").result().error() == "Invalid bookmark URL");
  CHECK_FALSE(peer->savedBookmark);
  peer->actionError = "Browser rejected action";
  CHECK(service.duplicateTab("one", 7).result().error() == "Browser rejected action");
}

TEST_CASE_METHOD(BrowserServiceTest, "disconnecting a stale browser peer preserves the reconnected profile") {
  auto previous = std::make_shared<TestBrowserPeer>();
  auto current = std::make_shared<TestBrowserPeer>();
  service.registerBrowser({.id = "profile", .peer = previous});
  service.setTabs("profile", {{.id = 7}});
  service.registerBrowser({.id = "profile", .peer = current});
  service.unregisterBrowser("profile", previous);
  REQUIRE(service.browser("profile"));
  REQUIRE(service.setPinned("profile", 7, true).result());
  CHECK(current->pinnedTab == 7);
  CHECK(previous->pinnedTab == 0);
  service.unregisterBrowser("profile", current);
  CHECK_FALSE(service.browser("profile"));
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
