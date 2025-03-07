#pragma once

#include "app-database.hpp"
#include "omnicast.hpp"
#include "quicklist-database.hpp"
class QuickLinkSeeder {
  Service<AppDatabase> appDb;
  Service<QuicklistDatabase> linkDb;

public:
  QuickLinkSeeder(Service<AppDatabase> appDb,
                  Service<QuicklistDatabase> quicklinkDb)
      : appDb(appDb), linkDb(quicklinkDb) {}

  bool seed() {
    auto browser = appDb.defaultBrowser();

    if (!browser)
      return false;

    linkDb.insertLink({.name = "google",
                       .icon = ":/assets/icons/google.svg",
                       .link = "https://www.google.com/search?q={query}",
                       .app = browser->id});
    linkDb.insertLink({.name = "duckduckgo",
                       .icon = ":/assets/icons/duckduckgo.svg",
                       .link = "https://www.duckduckgo.com/search?q={query}",
                       .app = browser->id});

    return true;
  }
};
