#pragma once
#include "omni-database.hpp"
#include <qcborcommon.h>
#include <qlogging.h>
#include <qdebug.h>
#include <qobject.h>
#include <qsqlquery.h>
#include <qstring.h>
#include <qsqlerror.h>
#include <qtmetamacros.h>

class Bookmark {
  /**
   * A list of reserved placeholder IDs. Each ID is meant to implement its own expansion rules.
   * A placeholder with an ID not present in the list is assumed to be an argument placeholder with the
   * unknown ID as a name. For instance, one can use `https://example.com/search?q={query}` instead of the
   * longer `https://example.com/search?q={argument name="query"}`.
   */
  const std::vector<QString> m_reservedPlaceholderIds = {"clipboard", "selected", "uuid", "date"};

public:
  struct ParsedPlaceholder {
    QString id;
    struct {
      int start;
      int end;
    } pos;
    std::map<QString, QString> args;
  };

  using UrlPart = std::variant<QString, ParsedPlaceholder>;

  struct Argument {
    QString name;
    QString defaultValue;
  };

private:
  std::vector<ParsedPlaceholder> m_placeholders;
  std::vector<Argument> m_args;
  std::vector<UrlPart> m_parts;
  QString m_raw;
  int m_id;
  QString m_name;
  QString m_app;
  QString m_icon;

  void insertPlaceholder(const ParsedPlaceholder &placeholder) {
    bool isReserved =
        std::ranges::any_of(m_reservedPlaceholderIds, [&](const QString &s) { return s == placeholder.id; });
    bool isArgument = !isReserved || placeholder.id == "argument";

    if (isArgument) {
      Argument arg;

      if (!isReserved) arg.name = placeholder.id;
      if (auto it = placeholder.args.find("name"); it != placeholder.args.end()) { arg.name = it->second; }
      if (auto it = placeholder.args.find("default"); it != placeholder.args.end()) {
        arg.defaultValue = it->second;
      }

      m_args.emplace_back(arg);
    }

    m_placeholders.emplace_back(placeholder);
  }

public:
  const std::vector<ParsedPlaceholder> &placeholders() const { return m_placeholders; }
  const std::vector<Argument> &arguments() const { return m_args; }

  int id() const { return m_id; }
  QString url() const { return m_raw; }

  /**
   * The ID of the application that is configured to open this url.
   */
  QString app() const { return m_app; }
  QString name() const { return m_name; }
  QString icon() const { return m_icon; }
  std::vector<UrlPart> parts() const { return m_parts; }

  void setApp(const QString &app) { m_app = app; }
  void setName(const QString &name) { m_name = name; }
  void setIcon(const QString &icon) { m_icon = icon; }
  void setId(int id) { m_id = id; }

  void setLink(const QString &link) {
    enum {
      BK_NORMAL,
      PH_ID,
      PH_KEY_START,
      PH_KEY,
      PH_VALUE_START,
      PH_VALUE,
      PH_VALUE_QUOTED
    } state = BK_NORMAL;
    size_t i = 0;
    size_t startPos = 0;
    ParsedPlaceholder parsed;
    std::pair<QString, QString> arg;

    m_placeholders.clear();
    m_args.clear();
    m_raw = link;

    while (i < link.size()) {
      QChar ch = link.at(i);

      switch (state) {
      case BK_NORMAL:
        if (ch == '{') {
          m_parts.emplace_back(link.sliced(startPos, i - startPos));
          state = PH_ID;
          startPos = i + 1;
        }
        break;
      case PH_ID:
        if (!ch.isLetterOrNumber()) {
          parsed.id = link.sliced(startPos, i - startPos);
          startPos = i--;
          state = PH_KEY_START;
        }
        break;
      case PH_KEY_START:
        if (ch == '}') {
          m_parts.emplace_back(parsed);
          insertPlaceholder(parsed);
          startPos = i + 1;
          state = BK_NORMAL;
          break;
        }
        if (!ch.isSpace()) {
          startPos = i--;
          arg.first.clear();
          arg.second.clear();
          state = PH_KEY;
        }
        break;
      case PH_KEY:
        if (ch == '=') {
          arg.first = link.sliced(startPos, i - startPos);
          qDebug() << "key" << arg.first;
          state = PH_VALUE_START;
        }
        break;
      case PH_VALUE_START:
        if (!ch.isSpace()) {
          startPos = i--;
          state = PH_VALUE;
        }
        break;
      case PH_VALUE:
        if (ch == '"') {
          arg.second += link.sliced(startPos, i - startPos);
          startPos = i + 1;
          state = PH_VALUE_QUOTED;
          break;
        }
        if (!ch.isLetterOrNumber()) {
          arg.second += link.sliced(startPos, i - startPos);
          qDebug() << "value" << arg.second;
          parsed.args.insert(arg);
          --i;
          state = PH_KEY_START;
        }
        break;
      case PH_VALUE_QUOTED:
        if (ch == '"') {
          arg.second += link.sliced(startPos, i - startPos);
          startPos = i + 1;
          state = PH_VALUE;
        }
      }

      ++i;
    }

    if (state == BK_NORMAL && i - startPos > 0) { m_parts.emplace_back(link.sliced(startPos, i - startPos)); }
  }

  Bookmark() {}
};

class BookmarkService : public QObject {
  Q_OBJECT

  OmniDatabase &m_db;
  std::vector<std::shared_ptr<Bookmark>> m_bookmarks;

  std::vector<std::shared_ptr<Bookmark>> loadAll() {
    std::vector<std::shared_ptr<Bookmark>> bookmarks;
    QSqlQuery query = m_db.createQuery();

    query.prepare(R"(
		SELECT id, name, icon, url, app, open_count, created_at, last_used_at
		FROM bookmarks
	)");

    if (!query.exec()) {
      qCritical() << "Failed to execute loadAll query: " << query.lastError();
      return {};
    }

    while (query.next()) {
      Bookmark bookmark;

      bookmark.setId(query.value(0).toInt());
      bookmark.setName(query.value(1).toString());
      bookmark.setIcon(query.value(2).toString());
      bookmark.setLink(query.value(3).toString());
      bookmark.setApp(query.value(4).toString());
      bookmarks.emplace_back(std::make_shared<Bookmark>(bookmark));
    }

    return bookmarks;
  }

  void createTables() {
    auto query = m_db.createQuery();

    query.prepare(R"(
		CREATE TABLE IF NOT EXISTS bookmarks (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			name TEXT NOT NULL,
			icon TEXT NOT NULL,
			url TEXT NOT NULL,
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
  }

public:
  std::vector<std::shared_ptr<Bookmark>> bookmarks() const { return m_bookmarks; }

  bool save(const Bookmark &bookmark) {
    QSqlQuery query = m_db.createQuery();

    query.prepare(R"(
		INSERT INTO bookmarks (name, icon, url, app) VALUES (:name, :icon, :url, :app)
		ON CONFLICT (id) 
		DO UPDATE SET name = :name, icon = :icon, url = :url, app = :app
	)");
    query.bindValue(":name", bookmark.name());
    query.bindValue(":icon", bookmark.icon());
    query.bindValue(":url", bookmark.url());
    query.bindValue(":app", bookmark.app());

    if (!query.exec()) {
      qCritical() << "Failed to save bookmark" << query.lastError();
      return false;
    }

    bool isLoaded =
        std::ranges::any_of(m_bookmarks, [&](const auto &item) { return item->id() == bookmark.id(); });

    if (!isLoaded) { m_bookmarks.emplace_back(std::make_shared<Bookmark>(bookmark)); }

    emit bookmarkSaved(bookmark);
    return true;
  }

  BookmarkService(OmniDatabase &db) : m_db(db) {
    createTables();
    m_bookmarks = loadAll();
    for (const auto &bookmark : m_bookmarks) {
      qCritical() << "book" << bookmark->name();
    }
    qCritical() << "loaded" << m_bookmarks.size() << "bookmarks";
  }

signals:
  void bookmarkSaved(const Bookmark &bookmark) const;
};
