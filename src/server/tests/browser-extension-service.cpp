#include <catch2/catch_test_macros.hpp>

#include "services/browser-extension-service.hpp"

TEST_CASE("browser metadata updates preserve tabs and refresh their labels") {
  BrowserExtensionService service;
  int changed = 0;
  QObject::connect(&service, &BrowserExtensionService::tabsChanged, &service, [&changed]() { ++changed; });

  service.registerBrowser({.id = "browser", .name = "Chromium", .engine = "chromium"});
  service.setTabs("browser", {{.id = 7, .title = "Example", .url = "https://example.com"}});
  REQUIRE(service.tabs().size() == 1);
  CHECK(service.tabs().front().browserLabel == "Chromium");

  service.registerBrowser({.id = "browser", .name = "Brave · Work", .engine = "chromium"});
  REQUIRE(service.tabs().size() == 1);
  CHECK(service.tabs().front().browserLabel == "Brave · Work");
  CHECK(service.tabs().front().id == 7);
  CHECK(changed == 3);
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
