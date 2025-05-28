#pragma once
#include "services/bookmark/bookmark.hpp"
#include "omni-database.hpp"
#include <qlogging.h>
#include <qdebug.h>
#include <qobject.h>
#include <qsqlquery.h>
#include <qstring.h>
#include <qsqlerror.h>
#include <qtmetamacros.h>

class BookmarkService : public QObject {
  Q_OBJECT

  OmniDatabase &m_db;
  std::vector<std::shared_ptr<Bookmark>> m_bookmarks;
  std::vector<std::shared_ptr<Bookmark>> loadAll();
  void createTables();

public:
  std::vector<std::shared_ptr<Bookmark>> bookmarks() const;

  bool removeBookmark(int id);
  bool createBookmark(const QString &name, const QString &icon, const QString &url, const QString &app);

  BookmarkService(OmniDatabase &db);

signals:
  void bookmarkSaved(const Bookmark &bookmark) const;
  void bookmarkRemoved(int id);
};
