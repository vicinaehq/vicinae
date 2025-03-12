#include "clipboard-service.hpp"

bool ClipboardService::setPinned(int id, bool pinned) {
  QSqlQuery query(db);

  if (pinned) {
    query.prepare("UPDATE history SET pinned_at = unixepoch() WHERE id = :id");
  } else {
    query.prepare("UPDATE history SET pinned_at = NULL WHERE id = :id");
  }

  query.bindValue(":id", id);

  return query.exec();
}

bool ClipboardService::indexTextDocument(int id, const QByteArray &buf) {
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

void ClipboardService::copyText(const QString &text) {
  QClipboard *clipboard = QApplication::clipboard();

  clipboard->setText(text);
}

int ClipboardService::insertHistoryLine(const InsertClipboardHistoryLine &payload) {
  QSqlQuery query(db);

  query.prepare(R"(
		INSERT INTO history (mime_type, text_preview, content_hash_md5)
		VALUES (:mime_type, :text_preview, :content_hash_md5)
		RETURNING id;
	)");
  query.bindValue(":mime_type", payload.mimeType);
  query.bindValue(":text_preview", payload.textPreview);
  query.bindValue(":content_hash_md5", payload.md5sum);

  if (!query.exec() || !query.next()) {
    qDebug() << "Failed to insert clipboard history line";
    return -1;
  }

  return query.value(0).toInt();
}

bool ClipboardService::copy(const QByteArray &data) {
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
  QString textPreview = isText ? data.sliced(0, qMin(data.size(), 50)) : "Image";
  auto entry = InsertClipboardHistoryLine{
      .mimeType = mime.name(),
      .textPreview = textPreview,
      .md5sum = md5sum,
  };

  qDebug() << "Storing content of type" << mime.name() << data.constData();

  int id = insertHistoryLine(entry);

  if (id == -1) { return false; }
  if (isText) {
    if (!indexTextDocument(id, data)) { qDebug() << "Failed to index document"; }
  }

  emit itemCopied(entry);

  return true;
}

std::vector<ClipboardHistoryEntry> ClipboardService::collectedSearch(const QString &q) {
  std::vector<ClipboardHistoryEntry> entries;
  QSqlQuery query(db);
  auto qstring = QString(R"(
		SELECT
			h.id, h.mime_type, h.text_preview, h.pinned_at, h.content_hash_md5, h.created_at
		FROM
			history as h
		JOIN 
			history_fts fts ON h.id = fts.history_id
		WHERE
			history_fts MATCH '%1'
		ORDER BY 
			h.pinned_at DESC,
			h.created_at DESC
	)")
                     .arg(q);

  if (!query.exec(qstring)) {
    qDebug() << "query failed" << query.lastError();
    return {};
  }

  while (query.next()) {
    auto sum = query.value(4).toString();

    entries.push_back(ClipboardHistoryEntry{
        .id = query.value(0).toInt(),
        .mimeType = query.value(1).toString(),
        .textPreview = query.value(2).toString(),
        .pinnedAt = query.value(3).toULongLong(),
        .md5sum = sum,
        .createdAt = query.value(5).toULongLong(),
        .filePath = _data_dir.absoluteFilePath(sum),
    });
  }

  qDebug() << "found" << entries.size();

  return entries;
}

PaginatedResponse<ClipboardHistoryEntry> ClipboardService::listAll(int limit, int offset) const {
  QSqlQuery query(db);

  if (!query.exec("SELECT COUNT(*) FROM history;") || !query.next()) { return {}; }

  PaginatedResponse<ClipboardHistoryEntry> response;

  response.totalCount = query.value(0).toInt();
  response.totalPages = ceil(static_cast<double>(response.totalCount) / limit);
  response.currentPage = ceil(static_cast<double>(offset) / limit);
  response.data.reserve(limit);

  query.prepare(R"(
	  	SELECT
			id, mime_type, text_preview, pinned_at, content_hash_md5, created_at
		FROM
			history
		ORDER BY 
			pinned_at DESC,
			created_at DESC
		LIMIT :limit
		OFFSET :offset
	  )");
  query.bindValue(":limit", limit);
  query.bindValue(":offset", offset);

  if (!query.exec()) { return {}; }

  while (query.next()) {
    auto sum = query.value(4).toString();

    response.data.push_back(ClipboardHistoryEntry{
        .id = query.value(0).toInt(),
        .mimeType = query.value(1).toString(),
        .textPreview = query.value(2).toString(),
        .pinnedAt = query.value(3).toULongLong(),
        .md5sum = sum,
        .createdAt = query.value(5).toULongLong(),
        .filePath = _data_dir.absoluteFilePath(sum),
    });
  }

  return response;
}

void ClipboardService::saveSelection(const ClipboardSelection &selection) {
  qDebug() << "save selection of size" << selection.offers.size();
}

ClipboardService::ClipboardService(const QString &path)
    : db(QSqlDatabase::addDatabase("QSQLITE")), _path(path),
      _data_dir(_path.dir().filePath("clipboard-data")) {
  db.setDatabaseName(path);

  if (!db.open()) { throw std::runtime_error("Failed to open clipboard db"); }

  _data_dir.mkpath(_data_dir.path());

  QSqlQuery query(db);

  query.exec(R"(
		CREATE TABLE IF NOT EXISTS selection (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			selection_hash_md5 TEXT NOT NULL,
			preferred_mime_type TEXT NOT NULL,
			source TEXT,
			created_at INTEGER DEFAULT (unixepoch()),
			pinned_at INTEGER DEFAULT 0
		);

		CREATE TABLE IF NOT EXISTS data_offer (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			mime_type TEXT NOT NULL,
			text_preview TEXT,
			content_hash_md5 TEXT NOT NULL,
		)
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
