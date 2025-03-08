#pragma once

#include "app-database.hpp"
#include "app.hpp"
#include "omni-icon.hpp"

class QuickLinkSeeder {
  Service<AppDatabase> appDb;
  Service<QuicklistDatabase> linkDb;

public:
  QuickLinkSeeder(Service<AppDatabase> appDb, Service<QuicklistDatabase> quicklinkDb)
      : appDb(appDb), linkDb(quicklinkDb) {}

  bool seed() {
    auto browser = appDb.defaultBrowser();

    if (!browser) { return false; }

    linkDb.insertLink({.name = "google",
                       .icon = FaviconOmniIconUrl("google.com").toString(),
                       .link = "https://www.google.com/search?q={query}",
                       .app = browser->id});
    linkDb.insertLink({.name = "duckduckgo",
                       .icon = FaviconOmniIconUrl("duckduckgo.com").toString(),
                       .link = "https://www.duckduckgo.com/search?q={query}",
                       .app = browser->id});
    linkDb.insertLink({.name = "Qwant",
                       .icon = FaviconOmniIconUrl("qwant.com").toString(),
                       .link = "https://www.qwant.com/?q={query}&t=web",
                       .app = browser->id});
    linkDb.insertLink({.name = "Brave Search",
                       .icon = FaviconOmniIconUrl("search.brave.com").toString(),
                       .link = "https://search.brave.com/search?q={query}&source=web",
                       .app = browser->id});
    linkDb.insertLink({.name = "Search Bing",
                       .icon = FaviconOmniIconUrl("www.bing.com").toString(),
                       .link = "https://www.bing.com/search?q={query}",
                       .app = browser->id});

    return true;
  }
};
