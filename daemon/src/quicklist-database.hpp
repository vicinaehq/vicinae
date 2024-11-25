#pragma once
#include "index-command.hpp"
#include <QString>
#include <memory>
#include <qlist.h>
#include <qlogging.h>

struct Quicklink : public IActionnable {
  QString displayName;
  QString url;
  QString iconName;
  QString name;
  QList<QString> placeholders;

  struct Open : public IAction {
    const Quicklink &ref;

    Open(const Quicklink &ref) : ref(ref) {}

    QString name() const override { return "Open link"; }
    void open(const QList<QString> args) const {
      QString url = QString(ref.url);

      for (size_t i = 0; i < args.size(); ++i) {
        url = url.arg(args.at(i));
      }

      xdgOpen(url.toLatin1().data());
    }
  };

  ActionList generateActions() const override {
    return {std::make_shared<Open>(*this)};
  }

  Quicklink(const QString &displayName, const QString &url,
            const QString &iconName, const QString &name,
            const QList<QString> &placeholders)
      : displayName(displayName), url(url), iconName(iconName), name(name),
        placeholders(placeholders) {}
};

struct QuicklistDatabase {
  QList<Quicklink> links;

public:
  QuicklistDatabase() {
    links.push_back(
        Quicklink("Search Google", "https://www.google.com/search?q=%1",
                  ":/assets/icons/google.svg", "google", {"query"}));
    links.push_back(Quicklink{"Search DuckDuckGo",
                              "https://www.duckduckgo.com/search?q={query}",
                              ":/assets/icons/duckduckgo.svg",
                              "ddg",
                              {"query"}});
    links.push_back(Quicklink{"Search Wikipedia",
                              "https://www.wikipedia.com/w?search=%1",
                              ":/assets/icons/wikipedia.svg",
                              "wikipedia",
                              {"query"}});
    links.push_back(Quicklink{"NPM",
                              "https://www.npm.com/search?q={query}",
                              ":/assets/icons/npm.svg",
                              "npm",
                              {"package"}});
  }
};
