#pragma once
#include <qcontainerfwd.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qlogging.h>
#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qsqlquery.h>
#include <qstack.h>
#include <qthread.h>
#include <qtmetamacros.h>
#include <stdexcept>

static const char *insertFileQuery = R"(
INSERT INTO files (name, path, mtime, type, parent_path, mime) VALUES (:name, :path, :mtime, :type, :parent_path, :mime);
)";

static const char *searchFileQuery = R"(
SELECT files.name, files.path, files.mime, files.type FROM files JOIN fts ON files.id = fts.rowid WHERE fts.name MATCH :query || '*' LIMIT :limit;
)";

static const char *listDirectoryQuery = R"(
SELECT path, mtime FROM files WHERE parent_path = :path
)";

// Expected to be slow, but we don't care too much about deletion performance.
static const char *deleteFileQuery = R"(
DELETE FROM files WHERE path LIKE ? || '%';
)";

static const char *filesTable = R"(
CREATE TABLE IF NOT EXISTS files (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	name TEXT NOT NULL,
	path TEXT NOT NULL UNIQUE,
	parent_path TEXT,
	mtime INTEGER,
	type INTEGER,
	mime TEXT
)
)";

static const char *fts5Tables = R"(
CREATE VIRTUAL TABLE fts USING fts5(name, content="files", content_rowid="id", tokenize = 'unicode61');
)";

static const char *fts5InsertTrigger = R"(
CREATE TRIGGER tbl_ai AFTER INSERT ON files BEGIN
  INSERT INTO fts(rowid, name) VALUES (new.id, new.name);
END;
)";

static const char *fts5DeleteTrigger = R"(
CREATE TRIGGER tbl_ad AFTER DELETE ON files BEGIN
  INSERT INTO fts(fts, rowid, name) VALUES('delete', old.id, old.name);
END;
)";

static const QList<QString> initScript = {"PRAGMA journal_mode=WAL",
                                          "PRAGMA synchronous=NORMAL",
                                          filesTable,
                                          fts5Tables,
                                          fts5InsertTrigger,
                                          fts5DeleteTrigger};

enum FileType { RegularFile, Directory };

struct FileInfo {
  QString name;
  QString path;
  QDateTime mtime;
  FileType type;
  QString parentPath;
  QString mime;
};

struct IndexedEntry {
  QString path;
  ulong mtime;
};

class FilesystemDatabase {
  QSqlDatabase db;

public:
  FilesystemDatabase(const QString &path,
                     const QString &connectionName = "files")
      : db(QSqlDatabase::addDatabase("QSQLITE", connectionName)) {
    db.setDatabaseName(path);

    if (!db.open()) {
      qDebug() << "Could not open database";
      return;
    }

    if (db.tables().isEmpty()) {
      QSqlQuery query(db);

      for (const auto &statement : initScript) {
        qDebug() << statement;
        if (!query.exec(statement)) {
          qDebug() << "Failed to init database!" << query.lastError();
          throw std::runtime_error("DB BAD!");
          return;
        }
      }

      db.commit();
      qDebug() << "initialized DB at" << path << connectionName;
    }
  }

  QList<IndexedEntry> listIndexedDirectory(const QString &directoryPath) {
    QSqlQuery query(db);

    if (!query.prepare(listDirectoryQuery)) {
      qDebug() << "listIndexedDirectory: failed to prepare"
               << query.lastError();
    }

    query.bindValue(":path", directoryPath);

    qDebug() << directoryPath;

    if (!query.exec()) {
      qDebug() << "listIndexedDirectory" << query.lastError();
      return {};
    }

    QList<IndexedEntry> entries;

    while (query.next()) {
      IndexedEntry entry;

      entry.path = query.value(0).toString();
      entry.mtime = query.value(1).toULongLong();
      entries << entry;
    }

    return entries;
  }

  bool insert(const FileInfo &info) { return insert(QList<FileInfo>{info}); }

  bool insert(const QList<FileInfo> &batch) {
    bool ok = true;
    QSqlQuery query(db);
    QVariantList names, paths, mtimes, types, parentPaths, mime;

    if (!db.transaction()) {
      qDebug() << "FilesystemDatabase::insert: failed to start transaction"
               << query.lastError();
      return false;
    }

    for (const auto &info : batch) {
      names << info.name;
      paths << info.path;
      mtimes << info.mtime.toSecsSinceEpoch();
      types << info.type;
      parentPaths << info.parentPath;
      mime << info.mime;
    }

    query.prepare(insertFileQuery);

    query.bindValue(":name", names);
    query.bindValue(":path", paths);
    query.bindValue(":mtime", mtimes);
    query.bindValue(":type", types);
    query.bindValue(":parent_path", parentPaths);
    query.bindValue(":mime", mime);
    ok = query.execBatch();

    if (!ok) {
      qDebug() << "failed to insert" << query.lastError();
    }

    if (!db.commit()) {
      qDebug() << "FilesystemDatabase::insert: failed to commit transaction"
               << query.lastError();
      return false;
    }

    return ok;
  }

  void remove(const FileInfo &info) {}

  QList<FileInfo> search(const QString &q, uint limit = 30) {
    QList<FileInfo> results;
    QSqlQuery query(db);

    query.prepare(searchFileQuery);
    query.bindValue(":query", q);
    query.bindValue(":limit", limit);
    query.exec();

    while (query.next()) {
      FileInfo info;

      info.name = query.value(0).toString();
      info.path = query.value(1).toString();
      info.mime = query.value(2).toString();
      info.type = static_cast<FileType>(query.value(3).toInt());
      results << info;
    }

    return results;
  }
};
