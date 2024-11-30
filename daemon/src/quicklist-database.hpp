#pragma once
#include "app-database.hpp"
#include "common.hpp"
#include <QSqlError>
#include <QString>
#include <memory>
#include <qdatetime.h>
#include <qdir.h>
#include <qhash.h>
#include <qlist.h>
#include <qlogging.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>

struct Quicklink : public IActionnable {
  uint id;
  QString app;
  uint openCount;
  QString displayName;
  QString url;
  QString iconName;
  QString name;
  QList<QString> placeholders;
  std::optional<QDateTime> lastUsedAt;

  struct Open : public IAction {
    const Quicklink &ref;

    Open(const Quicklink &ref) : ref(ref) {}

    QString name() const override { return "Open link"; }

    bool open(std::shared_ptr<DesktopExecutable> app,
              const QList<QString> args) const {
      QString url = QString(ref.url);

      for (size_t i = 0; i < args.size(); ++i) {
        url = url.arg(args.at(i));
      }

      return app->launch({url});
    }
  };

  ActionList generateActions() const override {
    return {std::make_shared<Open>(*this)};
  }

  Quicklink(uint id, const QString &name, const QString &url,
            const QString &icon, const QString &app, uint openCount,
            std::optional<QDateTime> lastUsedAt)
      : id(id), app(app), openCount(openCount), lastUsedAt(lastUsedAt) {
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

class QuicklistDatabase {
  QSqlDatabase db;
  QList<std::shared_ptr<Quicklink>> links;
  using List = QList<std::shared_ptr<Quicklink>>;

  Quicklink *findMutableById(uint id) {
    for (auto &link : links) {
      if (link->id == id)
        return link.get();
    }

    return nullptr;
  }

public:
  const List &list() { return links; }

  bool removeOne(uint id) {
    QSqlQuery query(db);

    query.prepare("DELETE FROM links WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
      qDebug() << "query.exec() failed";
      return false;
    }

    links.removeIf([id](auto &link) { return link->id == id; });

    return true;
  }

  List reloadAll() {
    List links;
    QSqlQuery query(db);

    query.prepare(R"(
		SELECT id, name, link, icon, app, open_count, last_used_at 
		FROM links
	)");

    if (!query.exec()) {
      qDebug() << "query.exec() failed";
      return links;
    }

    while (query.next()) {
      auto id = query.value(0).toUInt();
      auto name = query.value(1).toString();
      auto link = query.value(2).toString();
      auto icon = query.value(3).toString();
      auto app = query.value(4).toString();
      auto openCount = query.value(5).toUInt();
      auto lastUsedAtField = query.value(6);

      std::optional<QDateTime> lastUsedAt;

      if (!lastUsedAtField.isNull()) {
        lastUsedAt =
            QDateTime::fromSecsSinceEpoch(lastUsedAtField.toULongLong());
      }

      links.push_back(std::make_shared<Quicklink>(
          Quicklink{id, name, link, icon, app, openCount, lastUsedAt}));
    }

    return links;
  }

  bool incrementOpenCount(uint id) {
    auto link = findMutableById(id);

    if (!link)
      return false;

    QSqlQuery query(db);

    query.prepare(R"(
		UPDATE links 
		SET 
			open_count = open_count + 1, 
			last_used_at = unixepoch() 
		WHERE id = ?
	)");

    query.addBindValue(id);

    if (!query.exec()) {
      qDebug() << "query.exec() failed: " << query.lastError().text();
      return false;
    }

    link->openCount += 1;
    link->lastUsedAt = QDateTime::currentDateTime();

    return true;
  }

  void insertLink(const AddQuicklinkPayload &payload) {
    QSqlQuery query(db);

    query.prepare(R"(
		INSERT INTO links (name, icon, link, app) 
		VALUES (?, ?, ?, ?)
	)");
    query.addBindValue(payload.name);
    query.addBindValue(payload.icon);
    query.addBindValue(payload.link);
    query.addBindValue(payload.app);

    qDebug() << payload.name << payload.icon << payload.link << payload.app;

    if (!query.exec()) {
      qDebug() << "query.exec() failed: " << query.lastError().text();
      return;
    }

    auto link = std::make_shared<Quicklink>(
        Quicklink{query.lastInsertId().toUInt(), payload.name, payload.link,
                  payload.icon, payload.app, 0, std::nullopt});

    links.push_back(link);
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
			open_count INTEGER DEFAULT 0,
			created_at INTEGER DEFAULT (unixepoch()),
			last_used_at INTEGER
		);
	)");

    if (!query.exec()) {
      qDebug() << "Failed to execute initial query: " << query.lastError();
      return;
    }

    links = reloadAll();
  }
};
