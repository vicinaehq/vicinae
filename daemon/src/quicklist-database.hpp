#pragma once
#include <QString>
#include <qlist.h>

struct Quicklink {
  QString displayName;
  QString url;
  QString iconName;
  QString name;
  QList<QString> placeholders;
};

struct QuicklistDatabase {
  QList<Quicklink> links;

public:
  QuicklistDatabase() {
    links.push_back(Quicklink{.displayName = "Search Google",
                              .url = "https://www.google.com/search?q={query}",
                              .iconName = ":/assets/icons/google.svg",
                              .name = "google",
                              .placeholders = {"query"}});
    links.push_back(
        Quicklink{.displayName = "Search DuckDuckGo",
                  .url = "https://www.duckduckgo.com/search?q={query}",
                  .iconName = ":/assets/icons/duckduckgo.svg",
                  .name = "ddg",
                  .placeholders = {"query"}});
    links.push_back(Quicklink{.displayName = "NPM",
                              .url = "https://www.npm.com/search?q={query}",
                              .iconName = ":/assets/icons/npm.svg",
                              .name = "npm",
                              .placeholders = {"package"}});
  }
};
