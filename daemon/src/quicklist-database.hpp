#pragma once
#include "common.hpp"
#include <QSqlError>
#include <QString>
#include <memory>
#include <qdir.h>
#include <qlist.h>
#include <qlogging.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>

struct Quicklink : public IActionnable {
  uint id;
  QString app;
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

  Quicklink(uint id, const QString &name, const QString &url,
            const QString &icon, const QString &app)
      : id(id), app(app) {
    size_t i = 0;
    int start = -1;
    QString fmtUrl;

    while (i < url.size()) {
      if (url.at(i) == '{') {
        start = ++i;
        continue;
      }

      if (url.at(i) == '}') {
        placeholders.push_back(url.mid(start, i - start));
        fmtUrl += '%' + QString::number(placeholders.size());
        start = -1;
        ++i;
        continue;
      }

      if (start == -1) {
        fmtUrl += url.at(i);
      }
      ++i;
    }

    qDebug() << "file=" << fmtUrl;

    this->displayName = name;
    this->name = name;
    this->iconName = icon;
    this->url = fmtUrl;
  }

  Quicklink(const QString &displayName, const QString &url,
            const QString &iconName, const QString &name,
            const QList<QString> &placeholders)
      : displayName(displayName), url(url), iconName(iconName), name(name),
        placeholders(placeholders) {}
};

struct AddQuicklinkPayload {
  const QString &name;
  const QString &icon;
  const QString &link;
  const QString &app;
};

struct QuicklistDatabase {
  QSqlDatabase db;

  QList<Quicklink> links;

public:
  bool remove(unsigned id) {
    QSqlQuery query(db);

    query.prepare(R"(
		DELETE FROM links WHERE id = ?
	)");

    query.addBindValue(id);

    if (!query.exec()) {
      qDebug() << "query.exec() failed";
      return false;
    }

    links.removeIf([id](auto &link) { return link.id == id; });
    return true;
  }

  QList<Quicklink> loadAll() {
    QList<Quicklink> links;
    QSqlQuery query(db);

    query.prepare(R"(
		SELECT id, name, link, icon, app FROM links
	)");

    if (!query.exec()) {
      qDebug() << "query.exec() failed";
    }

    while (query.next()) {
      links.push_back({
          query.value(0).toUInt(),
          query.value(1).toString(),
          query.value(2).toString(),
          query.value(3).toString(),
          query.value(4).toString(),
      });
    }

    return links;
  }

  void addLink(const AddQuicklinkPayload &payload) {
    QSqlQuery query(db);

    query.prepare(
        "INSERT INTO links (name, icon, link, app) VALUES (?, ?, ?, ?)");
    query.addBindValue(payload.name);
    query.addBindValue(payload.icon);
    query.addBindValue(payload.link);
    query.addBindValue(payload.app);

    qDebug() << payload.name << payload.icon << payload.link << payload.app;

    if (!query.exec()) {
      qDebug() << "query.exec() failed: " << query.lastError().text();
    }

    // todo: get actual id
    links.push_back(
        {100, payload.name, payload.link, payload.icon, payload.app});

    // links.push_back({payload.name, payload.link, payload.icon});
  }

  QuicklistDatabase(const QString &path)
      : db(QSqlDatabase::addDatabase("QSQLITE", "quicklinks")) {

    QFile file(path);

    bool isFirstInit = !file.exists();

    db.setDatabaseName(path);

    if (!db.open()) {
      qDebug() << "Failed to open quicklinks db";
    }

    QSqlQuery query(db);

    query.prepare(R"(
		CREATE TABLE IF NOT EXISTS links (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			name TEXT NOT NULL,
			icon TEXT NOT NULL,
			link TEXT NOT NULL,
			app TEXT NOT NULL,
			created_at INTEGER DEFAULT (unixepoch()),
			updated_at INTEGER DEFAULT (unixepoch())
		);
	)");

    if (!query.exec()) {
      qDebug() << "Failed to execute initial query: " << query.lastError();
      return;
    }

    if (isFirstInit) {
      addLink({.name = "google",
               .icon = ":/assets/icons/google.svg",
               .link = "https://www.google.com/search?q={query}",
               .app = ""});
      addLink({.name = "duckduckgo",
               .icon = ":/assets/icons/duckduckgo.svg",
               .link = "https://www.duckduckgo.com/search?q={query}",
               .app = ""});

      qDebug() << "Done initializing";
    } else {
      links = loadAll();
    }
  }
};
