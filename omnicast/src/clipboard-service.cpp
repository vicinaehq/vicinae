#include "clipboard-service.hpp"
#include "clipboard-manager.hpp"
#include <filesystem>
#include <numbers>
#include <qbytearrayview.h>
#include <qcontainerfwd.h>
#include <qcryptographichash.h>
#include <qdir.h>
#include <qevent.h>
#include <qimagereader.h>
#include <qlogging.h>
#include <qsqlquery.h>
#include <qtypes.h>

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

struct TransformedSelection {
  QString md5sum;
  QString mimeType;
  QString dataPath;
};

std::string ClipboardService::getSelectionPreferredMimeType(const ClipboardSelection &selection) const {
  enum SelectionPriority { Other, HtmlText, Text, GenericImage, ImageJpeg, ImagePng, ImageSvg } priority;
  std::string preferredMimeType;

  for (const auto &offer : selection.offers) {
    auto mimePriority = SelectionPriority::Other;

    if (offer.mimeType.starts_with("text/")) {
      if (offer.mimeType == "text/html") {
        mimePriority = SelectionPriority::HtmlText;
      } else if (offer.mimeType == "text/svg") {
        mimePriority = SelectionPriority::ImageSvg;
      } else {
        mimePriority = SelectionPriority::Text;
      }
    } else if (offer.mimeType.starts_with("image/")) {
      if (offer.mimeType == "image/jpg" || offer.mimeType == "image/jpeg") {
        mimePriority = SelectionPriority::ImageJpeg;
      } else if (offer.mimeType == "image/png") {
        mimePriority = SelectionPriority::ImagePng;
      } else {
        mimePriority = SelectionPriority::GenericImage;
      }
    } else {
      mimePriority = SelectionPriority::Other;
    }

    if (mimePriority > priority) {
      priority = mimePriority;
      preferredMimeType = offer.mimeType;
    }
  }

  return preferredMimeType;
}

void ClipboardService::saveSelection(const ClipboardSelection &selection) {
  std::vector<TransformedSelection> transformedOffers;
  char buf[1 << 16];
  QCryptographicHash selectionHash(QCryptographicHash::Md5);

  transformedOffers.reserve(selection.offers.size());

  for (const auto &offer : selection.offers) {
    TransformedSelection selection{.mimeType = offer.mimeType.c_str()};
    QFile file(offer.path);

    if (!file.open(QIODevice::ReadOnly)) {
      qDebug() << "Unable to open file" << offer.path;
      continue;
    }

    QCryptographicHash fileHash(QCryptographicHash::Md5);
    quint64 rc = 0;

    while ((rc = file.read(buf, sizeof(buf))) > 0) {
      fileHash.addData(QByteArrayView(buf, rc));
    }

    auto md5sum = fileHash.result();
    QFileInfo targetFile(_data_dir.filePath(md5sum.toHex()));

    if (!targetFile.exists()) {
      QFile::copy(offer.path.c_str(), targetFile.filePath());
      qDebug() << "renamed" << offer.path << "to" << targetFile;
    }

    QFile::remove(offer.path);

    selectionHash.addData(offer.mimeType);
    selectionHash.addData(md5sum);
    selection.dataPath = targetFile.filePath();
    selection.md5sum = md5sum.toHex();
    transformedOffers.push_back(selection);
  }

  bool isLastSelection = false;

  // TODO: check last inserted selection hash, do not go forward if it's identical to the current one

  std::string preferredMimeType = getSelectionPreferredMimeType(selection);
  std::vector<InsertClipboardHistoryLine> dbOffers;

  if (!db.transaction()) {
    qDebug() << "Failed to create transaction";
    return;
  }

  QSqlQuery query(db);

  query.prepare(R"(
  	INSERT INTO selection (hash_md5, preferred_mime_type)
	VALUES (:hash_md5, :preferred_mime_type)
	RETURNING id;
  )");
  query.bindValue(":hash_md5", selectionHash.result().toHex());
  query.bindValue(":preferred_mime_type", preferredMimeType.c_str());

  if (!query.exec()) {
    qDebug() << "Failed to selection";
    return;
  }

  if (!query.next()) {
    qDebug() << "Failed to get row";
    return;
  }

  int selectionId = query.value(0).toInt();

  for (const auto &offer : transformedOffers) {
    QString textPreview;

    if (offer.mimeType.startsWith("text/")) {
      QFile file(offer.dataPath);

      if (!file.open(QIODevice::ReadOnly)) { return; }

      auto data = file.readAll();

      textPreview = data.sliced(0, 20);
      // TODO: index text for selection
    }

    else if (offer.mimeType.startsWith("image/")) {
      QImageReader reader(offer.dataPath);

      if (auto size = reader.size(); size.isValid()) {
        textPreview = QString("Image (%1x%2)").arg(size.width()).arg(size.height());
      } else {
        textPreview = "Image";
      }
    } else {
      textPreview = "Binary data";
    }

    query.prepare(R"(
		INSERT INTO data_offer (selection_id, mime_type, text_preview, content_hash_md5)
		VALUES (:selection_id, :mime_type, :text_preview, :content_hash_md5)
  	)");
    query.bindValue(":selection_id", selectionId);
    query.bindValue(":mime_type", offer.mimeType);
    query.bindValue(":text_preview", textPreview);
    query.bindValue(":content_hash_md5", offer.mimeType);

    if (!query.exec()) { return; }

    qDebug() << "inserted" << offer.mimeType << "for selection" << selectionId;
  }

  db.commit();
}

ClipboardService::ClipboardService(const QString &path)
    : db(QSqlDatabase::addDatabase("QSQLITE", "clipboard")), _path(path),
      _data_dir(_path.dir().filePath("clipboard-data")) {
  db.setDatabaseName(path);

  if (!db.open()) { throw std::runtime_error("Failed to open clipboard db"); }

  _data_dir.mkpath(_data_dir.path());

  QSqlQuery query(db);

  bool exec = query.exec(R"(
	CREATE TABLE IF NOT EXISTS selection (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		hash_md5 TEXT NOT NULL,
		preferred_mime_type TEXT NOT NULL,
		source TEXT,
		created_at INTEGER DEFAULT (unixepoch()),
		pinned_at INTEGER DEFAULT 0
	);
	)");

  if (!exec) { throw std::runtime_error(query.lastError().databaseText().toUtf8().constData()); }

  query.exec(R"(
   CREATE TABLE IF NOT EXISTS data_offer (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		mime_type TEXT NOT NULL,
		text_preview TEXT,
		content_hash_md5 TEXT NOT NULL,
		selection_id INTEGER,
		FOREIGN KEY(selection_id) REFERENCES selection(id)
	);
  )");

  query.exec(R"(
		CREATE VIRTUAL TABLE history_fts USING fts5(
			content,
			history_id,
			tokenize='porter'
		);
	)");
}
