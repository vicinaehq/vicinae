#pragma once
#include "common.hpp"
#include <QClipboard>
#include <QSqlError>
#include <cmath>
#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <numbers>
#include <qapplication.h>
#include <qclipboard.h>
#include <qcryptographichash.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qlogging.h>
#include <qmimedatabase.h>
#include <qobject.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qstringview.h>
#include <qtmetamacros.h>

struct InsertClipboardHistoryLine {
  QString mimeType;
  QString textPreview;
  bool isPinned;
  QString md5sum;
};

struct ClipboardHistoryEntry {
  int id;
  QString mimeType;
  QString textPreview;
  bool isPinned;
  QString md5sum;
  uint64_t createdAt;
};

class ClipboardService : public QObject, public NonAssignable {
  Q_OBJECT
  QSqlDatabase db;
  QMimeDatabase _mimeDb;
  QFileInfo _path;
  QDir _data_dir;

public:
  ClipboardService(const QString &path)
      : db(QSqlDatabase::addDatabase("QSQLITE")), _path(path),
        _data_dir(_path.dir().filePath("clipboard-data")) {
    db.setDatabaseName(path);

    if (!db.open()) { throw std::runtime_error("Failed to open clipboard db"); }

    _data_dir.mkpath(_data_dir.path());

    QSqlQuery query(db);

    query.exec(R"(
		CREATE TABLE IF NOT EXISTS history (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			mime_type TEXT NOT NULL,
			text_preview TEXT,
			content_hash_md5 TEXT NOT NULL,
			is_pinned INTEGER DEFAULT 0,
			source TEXT,
			created_at INTEGER DEFAULT (unixepoch())
		);
	)");

    query.exec(R"(
		CREATE VIRTUAL TABLE history_fts USING fts5(
			content,
			history_id,
			tokenize='porter'
		);
	)");

    if (!query.exec()) { qDebug() << "Failed to execute initial query"; }
  }

  bool setPinned(int id, bool pinned) {
    QSqlQuery query(db);

    query.prepare(R"(
		UPDATE history SET is_pinned = :is_pinned WHERE id = :id
	)");

    query.bindValue(":is_pinned", pinned);
    query.bindValue(":id", id);

    return query.exec();
  }

  PaginatedResponse<ClipboardHistoryEntry> listAll(int limit = 100, int offset = 0) const {
    QSqlQuery query(db);

    if (!query.exec("SELECT COUNT(*) FROM history;") || !query.next()) { return {}; }

    PaginatedResponse<ClipboardHistoryEntry> response;

    response.totalCount = query.value(0).toInt();
    response.totalPages = ceil(static_cast<double>(response.totalCount) / limit);
    response.currentPage = ceil(static_cast<double>(offset) / limit);
    response.data.reserve(limit);

    query.prepare(R"(
	  	SELECT
			id, mime_type, text_preview, is_pinned, content_hash_md5, created_at
		FROM
			history
		ORDER BY 
			is_pinned DESC,
			created_at DESC
		LIMIT :limit
		OFFSET :offset
	  )");
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);

    if (!query.exec()) { return {}; }

    while (query.next()) {
      response.data.push_back(ClipboardHistoryEntry{
          .id = query.value(0).toInt(),
          .mimeType = query.value(1).toString(),
          .textPreview = query.value(2).toString(),
          .isPinned = query.value(3).toBool(),
          .md5sum = query.value(4).toString(),
          .createdAt = query.value(5).toULongLong(),
      });
    }

    return response;
  }

  bool indexTextDocument(int id, const QByteArray &buf) {
    QSqlQuery query(db);

    query.prepare(R"(
		INSERT INTO history_fts (history_id, content) VALUES (:history_id, :content);
	)");
    query.bindValue(":history_id", id);
    query.bindValue(":content", buf);

    if (query.exec()) {
      qDebug() << "stored" << buf.toStdString();
      return true;
    }

    return false;
  }

  std::vector<ClipboardHistoryEntry> collectedSearch(const QString &q) {
    std::vector<ClipboardHistoryEntry> entries;
    QSqlQuery query(db);
    auto qstring = QString(R"(
		SELECT
			h.id, h.mime_type, h.text_preview, h.is_pinned, h.content_hash_md5, h.created_at
		FROM
			history as h
		JOIN 
			history_fts fts ON h.id = fts.history_id
		WHERE
			history_fts MATCH '%1'
		ORDER BY 
			h.is_pinned DESC,
			h.created_at DESC
	)")
                       .arg(q);

    std::cout << qstring.toStdString();

    if (!query.exec(qstring)) {
      qDebug() << "query failed" << query.lastError();
      return {};
    }

    while (query.next()) {
      entries.push_back(ClipboardHistoryEntry{
          .id = query.value(0).toInt(),
          .mimeType = query.value(1).toString(),
          .textPreview = query.value(2).toString(),
          .isPinned = query.value(3).toBool(),
          .md5sum = query.value(4).toString(),
          .createdAt = query.value(5).toULongLong(),
      });
    }

    qDebug() << "found" << entries.size();

    return entries;
  }

  bool copy(const QByteArray &data) {
    auto mime = _mimeDb.mimeTypeForData(data);
    auto md5sum = QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();

    {
      QFile file(_data_dir.filePath(md5sum));

      if (!file.exists()) {
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
          qDebug() << "cannot open file";
          return false;
        }

        file.write(data);
      }
    }

    bool isText = mime.name() == "text/plain" || mime.inherits("text/plain");
    QString textPreview = isText ? data.sliced(0, 50) : "Image";
    auto entry = InsertClipboardHistoryLine{
        .mimeType = mime.name(),
        .textPreview = textPreview,
        .isPinned = false,
        .md5sum = md5sum,
    };

    int id = insertHistoryLine(entry);

    if (id == -1) { return false; }
    if (isText) {
      if (!indexTextDocument(id, data)) { qDebug() << "Failed to index document"; }
    }

    emit itemCopied(entry);

    return true;
  }

  int insertHistoryLine(const InsertClipboardHistoryLine &payload) {
    QSqlQuery query(db);

    query.prepare(R"(
		INSERT INTO history (mime_type, text_preview, content_hash_md5, is_pinned)
		VALUES (:mime_type, :text_preview, :content_hash_md5, :is_pinned)
		RETURNING id;
	)");
    query.bindValue(":mime_type", payload.mimeType);
    query.bindValue(":text_preview", payload.textPreview);
    query.bindValue(":content_hash_md5", payload.md5sum);
    query.bindValue(":is_pinned", payload.isPinned);

    if (!query.exec() || !query.next()) {
      qDebug() << "Failed to insert clipboard history line";
      return -1;
    }

    return query.value(0).toInt();
  }

  void copyText(const QString &text) {
    QClipboard *clipboard = QApplication::clipboard();

    clipboard->setText(text);
  }

signals:
  void itemCopied(const InsertClipboardHistoryLine &item) const;
};
