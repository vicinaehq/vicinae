#include "clipboard-db.hpp"
#include "crypto.hpp"
#include "utils/migration-manager/migration-manager.hpp"
#include "vicinae.hpp"
#include <qlogging.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>

static const std::vector<QString> DB_PRAGMAS = {"PRAGMA journal_mode = WAL", "PRAGMA synchronous = normal",
                                                "PRAGMA journal_size_limit = 6144000",
                                                "PRAGMA foreign_keys = ON"};

std::optional<ClipboardSelectionRecord> ClipboardDatabase::findSelection(const QString &id) {
  ClipboardSelectionRecord selection;
  QSqlQuery query(m_db);

  query.prepare("SELECT id, mime_type, encryption_type from data_offer where selection_id = :id");
  query.addBindValue(id);

  if (!query.exec()) {
    qCritical() << "Failed to retrieve selection for id" << id << query.lastError();
    return std::nullopt;
  }

  while (query.next()) {
    ClipboardSelectionOfferRecord record;

    record.id = query.value(0).toString();
    record.mimeType = query.value(1).toString();
    record.encryption = static_cast<ClipboardEncryptionType>(query.value(2).toUInt());
    selection.offers.emplace_back(record);
  }

  return selection;
}

PaginatedResponse<ClipboardHistoryEntry> ClipboardDatabase::query(int limit, int offset,
                                                                  const ClipboardListSettings &opts) const {

  PaginatedResponse<ClipboardHistoryEntry> response;
  response.data.reserve(limit);

  QString queryString;

  bool hasFilters = !opts.query.isEmpty() || opts.kind.has_value();

  // Use different query strategies based on whether we have filters:
  // - Without filters: Sort-before-join strategy allows the index on (pinned_at, updated_at)
  //   to be used for sorting before the expensive JOIN operation. This is critical for
  //   performance on large datasets where sorting after JOIN+GROUP BY
  //   would require materializing and sorting the entire result set.
  // - With filters: FTS search or kind filter already reduces the dataset significantly,
  //   so the standard join approach is acceptable.
  if (!hasFilters) {
    queryString = QString(R"(
      SELECT 
        s.id,
        o.mime_type, 
        o.text_preview, 
        s.pinned_at, 
        o.content_hash_md5, 
        s.updated_at, 
        o.size, 
        s.kind, 
        o.url_host,
        o.encryption_type,
        s.total_count
      FROM (
        SELECT id, pinned_at, updated_at, kind, preferred_mime_type,
               COUNT(*) OVER() as total_count
        FROM selection
        ORDER BY pinned_at DESC, updated_at DESC
        LIMIT %1 OFFSET %2
      ) s
      JOIN data_offer o
        ON o.selection_id = s.id
        AND o.mime_type = s.preferred_mime_type
    )")
                      .arg(limit)
                      .arg(offset);
  } else {
    queryString = R"(
      SELECT
        selection.id,
        o.mime_type, 
        o.text_preview, 
        pinned_at, 
        o.content_hash_md5, 
        updated_at, 
        o.size, 
        selection.kind, 
        o.url_host,
        o.encryption_type, 
        COUNT(*) OVER() as count
      FROM selection
      JOIN data_offer o
        ON o.selection_id = selection.id
        AND o.mime_type = selection.preferred_mime_type
    )";

    if (!opts.query.isEmpty()) {
      queryString += " JOIN selection_fts ON selection_fts.selection_id = selection.id ";
    }

    if (!opts.query.isEmpty()) { queryString += " WHERE selection_fts MATCH '\"" + opts.query + "\"*' "; }

    if (opts.kind) {
      if (opts.query.isEmpty()) {
        queryString += " WHERE selection.kind = :kind";
      } else {
        queryString += " AND selection.kind = :kind";
      }
    }

    queryString += " GROUP BY selection.id ";
    queryString += " ORDER BY pinned_at DESC, updated_at DESC";
    queryString = QString("SELECT * FROM (%1) LIMIT %2 OFFSET %3").arg(queryString).arg(limit).arg(offset);
  }

  QSqlQuery query(m_db);
  query.prepare(queryString);

  if (opts.kind) { query.bindValue(":kind", static_cast<quint8>(*opts.kind)); }

  if (!query.exec()) {
    qWarning() << "Failed to list all clipboard items" << query.lastError();
    return {};
  }

  while (query.next()) {
    ClipboardHistoryEntry dto{.id = query.value(0).toString(),
                              .mimeType = query.value(1).toString(),
                              .textPreview = query.value(2).toString(),
                              .pinnedAt = query.value(3).toULongLong(),
                              .md5sum = query.value(4).toString(),
                              .updatedAt = query.value(5).toULongLong(),
                              .size = query.value(6).toULongLong(),
                              .kind = static_cast<ClipboardOfferKind>(query.value(7).toUInt()),
                              .encryption = static_cast<ClipboardEncryptionType>(query.value(9).toUInt())};

    if (auto val = query.value(8); !val.isNull()) { dto.urlHost = val.toString(); }

    response.totalCount = query.value(10).toInt();
    response.data.push_back(dto);
  }

  response.totalPages = ceil(static_cast<double>(response.totalCount) / limit);
  response.currentPage = ceil(static_cast<double>(offset) / limit);

  return response;
}

std::optional<QString> ClipboardDatabase::retrieveKeywords(const QString &id) {
  QSqlQuery query(m_db);

  query.prepare("SELECT keywords FROM selection WHERE id = :id");
  query.bindValue(":id", id);

  if (!query.exec()) {
    qWarning() << "Failed to get keywords for selection" << id << query.lastError();
    return std::nullopt;
  }

  if (!query.next()) return std::nullopt;

  return query.value(0).toString();
}

bool ClipboardDatabase::setKeywords(const QString &id, const QString &keywords) {
  return transaction([&](auto *db) {
    QSqlQuery query(m_db);

    query.prepare("UPDATE selection SET keywords = :keywords WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":keywords", keywords);

    if (!query.exec()) {
      qWarning() << "Failed to set keywords for id" << id << query.lastError();
      return false;
    }

    return true;
  });
}

bool ClipboardDatabase::removeAll() {
  QSqlQuery query(m_db);

  return query.exec("DELETE FROM selection");
}

std::vector<QString> ClipboardDatabase::removeSelection(const QString &selectionId) {
  if (!m_db.transaction()) { return {}; }

  QSqlQuery query(m_db);

  query.prepare(R"(
  	DELETE FROM 
		data_offer
	WHERE 
		selection_id = :selection_id
	RETURNING id
  )");
  query.bindValue(":selection_id", selectionId);

  if (!query.exec()) {
    qDebug() << "failed to execute data_offer deletion" << query.lastError();
    m_db.rollback();
    return {};
  }

  std::vector<QString> deletedOffers;

  while (query.next()) {
    deletedOffers.emplace_back(query.value(0).toString());
  }

  query.prepare("DELETE FROM selection WHERE id = :selection_id");
  query.bindValue(":selection_id", selectionId);

  if (!query.exec()) {
    qDebug() << "failed to execute selecton deletion" << query.lastError();
    m_db.rollback();
    return {};
  }

  m_db.commit();

  return deletedOffers;
}

std::optional<PreferredClipboardOfferRecord>
ClipboardDatabase::findPreferredOffer(const QString &selectionId) {
  QSqlQuery query(m_db);

  query.prepare(R"(
		SELECT o.id, o.encryption_type FROM data_offer o
		JOIN selection s ON s.id = o.selection_id
		WHERE o.mime_type = s.preferred_mime_type
		AND selection_id = :selection
	)");
  query.addBindValue(selectionId);

  if (!query.exec()) {
    qCritical() << "Failed to decrypt main selection offer" << query.lastError();
    return {};
  }

  if (!query.next()) {
    qCritical() << "No match for default mime type" << query.lastError();
    return {};
  }

  QString id = query.value(0).toString();
  auto encryption = static_cast<ClipboardEncryptionType>(query.value(1).toUInt());

  return PreferredClipboardOfferRecord{.id = id, .encryption = encryption};
}

bool ClipboardDatabase::setPinned(const QString &id, bool pinned) {
  qint64 epoch = QDateTime::currentSecsSinceEpoch();
  QSqlQuery query(m_db);

  if (pinned) {
    query.prepare("UPDATE selection SET pinned_at = :epoch WHERE id = :id");
  } else {
    query.prepare("UPDATE selection SET pinned_at = NULL WHERE id = :id");
  }

  query.bindValue(":id", id);
  query.bindValue(":epoch", epoch);

  return query.exec();
}

bool ClipboardDatabase::insertSelection(const InsertSelectionPayload &payload) {
  QSqlQuery query(m_db);

  query.prepare(R"(
  	INSERT INTO selection (id, kind, offer_count, hash_md5, preferred_mime_type, source, created_at, updated_at)
	VALUES (:id, :kind, :offer_count, :hash_md5, :preferred_mime_type, :source, :epoch, :epoch)
	RETURNING id, created_at;
  )");
  query.bindValue(":id", payload.id);
  query.bindValue(":kind", static_cast<quint8>(payload.kind));
  query.bindValue(":offer_count", static_cast<uint>(payload.offerCount));
  query.bindValue(":hash_md5", payload.hash);
  query.bindValue(":preferred_mime_type", payload.preferredMimeType);
  query.bindValue(":epoch", QDateTime::currentSecsSinceEpoch());

  if (payload.source) { query.bindValue(":source", *payload.source); }

  if (!query.exec()) {
    qCritical() << "Failed to insert selection" << query.lastError();
    return false;
  }

  return true;
}

bool ClipboardDatabase::transaction(const TxHandle &handle) {
  if (!m_db.transaction()) {
    qCritical() << "Failed to start transaction" << m_db.lastError();
    return false;
  }

  if (handle(this)) { return m_db.commit(); }

  return m_db.rollback();
}

bool ClipboardDatabase::tryBubbleUpSelection(const QString &idLike) {
  qint64 updatedAt = QDateTime::currentSecsSinceEpoch();
  QSqlQuery query(m_db);

  query.prepare("UPDATE selection SET updated_at = :updated_at WHERE hash_md5 = :id OR id = :id");
  query.bindValue(":id", idLike);
  query.bindValue(":updated_at", updatedAt);

  if (!query.exec()) { qCritical() << "Failed to execute clipboard update"; }

  return query.numRowsAffected() > 0;
}

bool ClipboardDatabase::indexSelectionContent(const QString &selectionId, const QString &content) {
  QSqlQuery query(m_db);

  query.prepare(R"(
		INSERT INTO selection_fts (selection_id, content) VALUES (:selection_id, :content);
	)");
  query.bindValue(":selection_id", selectionId);
  query.bindValue(":content", content);

  if (!query.exec()) {
    qCritical() << "failed to index text" << query.lastError();
    return false;
  }

  return true;
}

void ClipboardDatabase::runMigrations() {
  MigrationManager manager(m_db, "clipboard");

  manager.runMigrations();
}

bool ClipboardDatabase::insertOffer(const InsertClipboardOfferPayload &payload) {
  QSqlQuery query(m_db);

  query.prepare(R"(
		INSERT INTO data_offer (id, selection_id, mime_type, text_preview, content_hash_md5, encryption_type, size, kind, url_host)
		VALUES (:id, :selection_id, :mime_type, :text_preview, :content_hash_md5, :encryption, :size, :kind, :url_host)
  	)");
  query.bindValue(":id", payload.id);
  query.bindValue(":selection_id", payload.selectionId);
  query.bindValue(":mime_type", payload.mimeType);
  query.bindValue(":text_preview", payload.textPreview);
  query.bindValue(":content_hash_md5", payload.md5sum);
  query.bindValue(":encryption", static_cast<quint8>(payload.encryption));
  query.bindValue(":size", payload.size);
  query.bindValue(":kind", static_cast<quint8>(payload.kind));

  if (payload.urlHost) { query.bindValue(":url_host", *payload.urlHost); }

  if (!query.exec()) {
    qCritical() << "Failed to inset offer" << query.lastError();
    return false;
  }

  return true;
}

ClipboardDatabase::ClipboardDatabase() {
  QString connId = QString("%1-%2").arg("clipboard").arg(Crypto::UUID::v4());
  m_db = QSqlDatabase::addDatabase("QSQLITE", connId);
  m_db.setDatabaseName((Omnicast::dataDir() / "clipboard.db").c_str());

  if (!m_db.open()) { qCritical() << "Failed to open database"; }

  QSqlQuery query(m_db);

  for (const auto &pragma : DB_PRAGMAS) {
    if (!query.exec(pragma)) { qCritical() << "Failed to execute pragma" << pragma << query.lastError(); }
  }
}

ClipboardDatabase::~ClipboardDatabase() {
  QString conn = m_db.connectionName();
  m_db.close();
  m_db = QSqlDatabase();
  QSqlDatabase::removeDatabase(conn);
}
