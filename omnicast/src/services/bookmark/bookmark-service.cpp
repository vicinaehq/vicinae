#include "services/bookmark/bookmark-service.hpp"
#include "bookmark-service.hpp"

std::vector<std::shared_ptr<Bookmark>> BookmarkService::loadAll() {
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

std::vector<std::shared_ptr<Bookmark>> BookmarkService::bookmarks() const { return m_bookmarks; }

bool BookmarkService::removeBookmark(int id) {
  QSqlQuery query = m_db.createQuery();

  query.prepare("DELETE FROM bookmarks WHERE id = :id");
  query.bindValue(":id", id);

  if (!query.exec()) {
    qCritical() << "Failed to remove bookmark" << query.lastError();
    return false;
  }

  auto it = std::ranges::remove_if(m_bookmarks, [id](const auto &mark) { return mark->id() == id; });

  m_bookmarks.erase(it.begin(), it.end());
  emit bookmarkRemoved(id);

  return true;
}

bool BookmarkService::createBookmark(const QString &name, const QString &icon, const QString &url,
                                     const QString &app) {
  QSqlQuery query = m_db.createQuery();

  query.prepare(R"(
		INSERT INTO bookmarks (name, icon, url, app) VALUES (:name, :icon, :url, :app)
		RETURNING id, name, icon, url, app
	)");
  query.bindValue(":name", name);
  query.bindValue(":icon", icon);
  query.bindValue(":url", url);
  query.bindValue(":app", app);

  if (!query.exec()) {
    qCritical() << "Failed to save bookmark" << query.lastError();
    return false;
  }

  if (!query.next()) { return false; }

  Bookmark bookmark;

  bookmark.setId(query.value(0).toInt());
  bookmark.setName(query.value(1).toString());
  bookmark.setIcon(query.value(2).toString());
  bookmark.setLink(query.value(3).toString());
  bookmark.setApp(query.value(4).toString());
  m_bookmarks.emplace_back(std::make_shared<Bookmark>(bookmark));
  emit bookmarkSaved(bookmark);

  return true;
}

BookmarkService::BookmarkService(OmniDatabase &db) : m_db(db) {
  m_bookmarks = loadAll();
  qCritical() << "loaded" << m_bookmarks.size() << "bookmarks";
}
