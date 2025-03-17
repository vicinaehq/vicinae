#include "clipboard/clipboard-service.hpp"
#include "clipboard/clipboard-server.hpp"
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <numbers>
#include <qbytearrayview.h>
#include "timer.hpp"
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
    query.prepare("UPDATE selection SET pinned_at = unixepoch() WHERE id = :id");
  } else {
    query.prepare("UPDATE selection SET pinned_at = NULL WHERE id = :id");
  }

  query.bindValue(":id", id);

  return query.exec();
}

void ClipboardService::copyText(const QString &text) {
  QClipboard *clipboard = QApplication::clipboard();

  clipboard->setText(text);
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

PaginatedResponse<ClipboardHistoryEntry> ClipboardService::listAll(int limit, int offset,
                                                                   const ClipboardListSettings &opts) const {
  QSqlQuery query(db);

  if (!query.exec("SELECT COUNT(*) FROM selection;") || !query.next()) { return {}; }

  PaginatedResponse<ClipboardHistoryEntry> response;

  response.totalCount = query.value(0).toInt();
  response.totalPages = ceil(static_cast<double>(response.totalCount) / limit);
  response.currentPage = ceil(static_cast<double>(offset) / limit);
  response.data.reserve(limit);

  QString queryString = R"(
	  	SELECT
			selection.id, o.mime_type, o.text_preview, pinned_at, o.content_hash_md5, created_at
		FROM
			selection
		JOIN
			data_offer o
		ON 
			o.selection_id = selection.id
		AND
			o.mime_type = selection.preferred_mime_type

	)";

  if (!opts.query.isEmpty()) {
    queryString += " JOIN selection_fts ON selection_fts.selection_id = selection.id ";
  }

  if (!opts.query.isEmpty()) { queryString += " WHERE selection_fts MATCH '" + opts.query + "*' "; }

  if (!opts.query.isEmpty()) { queryString += " GROUP BY selection.id "; }

  queryString += R"(
	ORDER BY
        pinned_at DESC,
        created_at DESC
    LIMIT :limit
    OFFSET :offset
  )";

  // qDebug() << "query" << queryString;

  query.prepare(queryString);
  query.bindValue(":limit", limit);
  query.bindValue(":offset", offset);

  Timer timer;

  if (!query.exec()) {
    qDebug() << "Failed to listall clipboard items" << query.lastError();
    return {};
  }

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
  timer.time("clipboard db query listAll");

  return response;
}

struct TransformedSelection {
  QString md5sum;
  QString mimeType;
  QString dataPath;
};

std::string ClipboardService::getSelectionPreferredMimeType(const ClipboardSelection &selection) const {
  enum SelectionPriority {
    Invalid,
    Other,
    HtmlText,
    Text,
    GenericImage,
    ImageJpeg,
    ImagePng,
    ImageSvg
  } priority = Invalid;
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

QString ClipboardService::createTextPreview(const QByteArray &data, int maxLength) const {
  auto s = QString(data).trimmed();
  QString preview;

  preview.reserve(maxLength);

  bool wasSpace = false;

  for (int i = 0; i < s.size() && preview.size() < maxLength; ++i) {
    auto c = s.at(i);

    if (c.isSpace()) {
      if (!wasSpace) { preview.append(' '); }
    } else {
      preview.append(c);
    }

    wasSpace = c.isSpace();
  }

  return preview;
}

bool ClipboardService::removeSelection(int selectionId) {
  if (!db.transaction()) { return false; }

  QSqlQuery query(db);

  query.prepare(R"(
  	DELETE FROM 
		data_offer
	WHERE 
		selection_id = :selection_id
	RETURNING 
		content_hash_md5,
		(
			SELECT
				COUNT(*)
			FROM
				data_offer do_b
			WHERE
				do_b.content_hash_md5 = data_offer.content_hash_md5
		) as remaining;
  )");
  query.bindValue(":selection_id", selectionId);

  if (!query.exec()) {
    qDebug() << "failed to execute data_offer deletion" << query.lastError();
    db.rollback();
    return false;
  }

  std::vector<QString> flaggedForDeletion;

  while (query.next()) {
    auto sum = query.value(0).toString();
    auto remaining = query.value(1).toUInt();

    if (remaining == 0) { flaggedForDeletion.push_back(sum); }
  }

  query.prepare("DELETE FROM selection WHERE id = :selection_id");
  query.bindValue(":selection_id", selectionId);

  if (!query.exec()) {
    qDebug() << "failed to execute selecton deletion" << query.lastError();
    db.rollback();
    return false;
  }

  db.commit();

  for (const auto &sum : flaggedForDeletion) {
    QFile::remove(_data_dir.filePath(sum));
  }

  return true;
}

void ClipboardService::saveSelection(const ClipboardSelection &selection) {
  std::vector<TransformedSelection> transformedOffers;
  char buf[1 << 16];
  QCryptographicHash selectionHash(QCryptographicHash::Md5);
  uint64_t totalLength = 0;
  ClipboardHistoryEntry insertedEntry;

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
      totalLength += rc;
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

  // probably a clipboard clear
  if (totalLength == 0) {
    qDebug() << "skipped clipboard clear";
    return;
  }

  auto selectionSum = selectionHash.result().toHex();

  if (!db.transaction()) {
    qDebug() << "Failed to create transaction" << db.lastError();
    return;
  }

  QSqlQuery query(db);

  query.exec("SELECT hash_md5 FROM selection ORDER BY created_at DESC LIMIT 1");

  if (query.next() && query.value(0).toString() == selectionSum) {
    qDebug() << "ignored immediate clip duplicate";
    db.rollback();
    return;
  }

  std::string preferredMimeType = getSelectionPreferredMimeType(selection);
  std::vector<InsertClipboardHistoryLine> dbOffers;

  query.prepare(R"(
  	INSERT INTO selection (offer_count, hash_md5, preferred_mime_type, source)
	VALUES (:offer_count, :hash_md5, :preferred_mime_type, :source)
	RETURNING id, created_at;
  )");
  query.bindValue(":offer_count", static_cast<uint>(selection.offers.size()));
  query.bindValue(":hash_md5", selectionHash.result().toHex());
  query.bindValue(":preferred_mime_type", preferredMimeType.c_str());

  // TODO: use window manager integration to figure out what's the currently focused window
  query.bindValue(":source", {});

  if (!query.exec()) {
    qDebug() << "Failed to selection" << query.lastError();
    db.rollback();
    return;
  }

  if (!query.next()) {
    qDebug() << "Failed to get row";
    db.rollback();
    return;
  }

  int selectionId = query.value(0).toInt();
  auto createdAt = query.value(1).toULongLong();

  for (const auto &offer : transformedOffers) {
    QString textPreview;

    if (offer.mimeType.startsWith("text/")) {
      QFile file(offer.dataPath);

      if (!file.open(QIODevice::ReadOnly)) { return; }

      auto data = file.readAll();

      textPreview = createTextPreview(data, 50);
      query.prepare(R"(
		INSERT INTO selection_fts (selection_id, content) VALUES (:selection_id, :content);
	)");
      query.bindValue(":selection_id", selectionId);
      query.bindValue(":content", data);

      if (!query.exec()) {
        qDebug() << "failed to index text" << query.lastError();
        db.rollback();
        return;
      }

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
      textPreview = "Unnamed";
    }

    query.prepare(R"(
		INSERT INTO data_offer (selection_id, mime_type, text_preview, content_hash_md5)
		VALUES (:selection_id, :mime_type, :text_preview, :content_hash_md5)
  	)");
    query.bindValue(":selection_id", selectionId);
    query.bindValue(":mime_type", offer.mimeType);
    query.bindValue(":text_preview", textPreview);
    query.bindValue(":content_hash_md5", offer.md5sum);

    if (!query.exec()) {
      db.rollback();
      return;
    }

    if (offer.mimeType == preferredMimeType) {
      insertedEntry.id = selectionId;
      insertedEntry.pinnedAt = 0;
      insertedEntry.createdAt = createdAt;
      insertedEntry.mimeType = offer.mimeType;
      insertedEntry.md5sum = offer.md5sum;
      insertedEntry.textPreview = textPreview;
      insertedEntry.filePath = _data_dir.filePath(offer.md5sum);
    }
  }

  db.commit();
  emit itemInserted(insertedEntry);
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
		offer_count TEXT,
		created_at INTEGER DEFAULT (unixepoch()),
		pinned_at INTEGER
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
		FOREIGN KEY(selection_id) 
		REFERENCES selection(id)
		ON DELETE CASCADE
	);
  )");

  query.exec(R"(
		CREATE VIRTUAL TABLE selection_fts USING fts5(
			content,
			selection_id,
			tokenize='porter'
		);
	)");
}
