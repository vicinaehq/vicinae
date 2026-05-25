#include "clipboard-db.hpp"
#include "utils/migration-manager/migration-manager.hpp"
#include "vicinae.hpp"
#include <qlogging.h>

static constexpr const char *CLIPBOARD_PRAGMAS[] = {
    "PRAGMA journal_mode = WAL", "PRAGMA synchronous = normal", "PRAGMA journal_size_limit = 6144000",
    "PRAGMA foreign_keys = ON"};

std::optional<ClipboardSelectionRecord> ClipboardDatabase::findSelection(const QString &id) {
  ClipboardSelectionRecord selection;

  auto stmt = m_db.prepare("SELECT id, mime_type, encryption_type from data_offer where selection_id = :id");
  stmt.bind(":id", id);

  while (stmt.step()) {
    ClipboardSelectionOfferRecord record;

    record.id = stmt.columnQString(0);
    record.mimeType = stmt.columnQString(1);
    record.encryption = static_cast<ClipboardEncryptionType>(stmt.columnInt(2));
    selection.offers.emplace_back(record);
  }

  auto srcStmt = m_db.prepare("SELECT source FROM selection WHERE id = :id");
  srcStmt.bind(":id", id);
  if (srcStmt.step() && !srcStmt.isNull(0)) { selection.source = srcStmt.columnQString(0); }

  return selection;
}

PaginatedResponse<ClipboardHistoryEntry> ClipboardDatabase::query(int limit, int offset,
                                                                  const ClipboardListSettings &opts) const {

  PaginatedResponse<ClipboardHistoryEntry> response;
  response.data.reserve(limit);

  QString queryString;

  bool const hasFilters = !opts.query.isEmpty() || opts.kind.has_value();

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

  auto stmt = m_db.prepare(queryString.toStdString());

  if (opts.kind) { stmt.bind(":kind", static_cast<int>(*opts.kind)); }

  while (stmt.step()) {
    ClipboardHistoryEntry dto{.id = stmt.columnQString(0),
                              .mimeType = stmt.columnQString(1),
                              .textPreview = stmt.columnQString(2),
                              .pinnedAt = stmt.columnUInt64(3),
                              .md5sum = stmt.columnQString(4),
                              .updatedAt = stmt.columnUInt64(5),
                              .size = stmt.columnUInt64(6),
                              .kind = static_cast<ClipboardOfferKind>(stmt.columnInt(7)),
                              .encryption = static_cast<ClipboardEncryptionType>(stmt.columnInt(9))};

    if (!stmt.isNull(8)) { dto.urlHost = stmt.columnQString(8); }

    response.totalCount = stmt.columnInt(10);
    response.data.push_back(dto);
  }

  response.totalPages = ceil(static_cast<double>(response.totalCount) / limit);
  response.currentPage = ceil(static_cast<double>(offset) / limit);

  return response;
}

std::optional<QString> ClipboardDatabase::retrieveKeywords(const QString &id) {
  auto stmt = m_db.prepare("SELECT keywords FROM selection WHERE id = :id");
  stmt.bind(":id", id);

  if (!stmt.step()) return std::nullopt;

  return stmt.columnQString(0);
}

bool ClipboardDatabase::setKeywords(const QString &id, const QString &keywords) {
  return transaction([&](auto *) {
    auto stmt = m_db.prepare("UPDATE selection SET keywords = :keywords WHERE id = :id");
    stmt.bind(":id", id);
    stmt.bind(":keywords", keywords);

    if (!stmt.exec()) {
      qWarning() << "Failed to set keywords for id" << id << stmt.lastError().c_str();
      return false;
    }

    return true;
  });
}

bool ClipboardDatabase::removeAll() {
  return m_db.exec("DELETE FROM selection_fts") && m_db.exec("DELETE FROM data_offer") &&
         m_db.exec("DELETE FROM selection");
}

std::vector<QString> ClipboardDatabase::removeSelection(const QString &selectionId) {
  auto tx = m_db.transaction();

  auto offerStmt = m_db.prepare(R"(
    DELETE FROM
      data_offer
    WHERE
      selection_id = :selection_id
    RETURNING id
  )");
  offerStmt.bind(":selection_id", selectionId);

  std::vector<QString> deletedOffers;

  while (offerStmt.step()) {
    deletedOffers.emplace_back(offerStmt.columnQString(0));
  }

  auto selStmt = m_db.prepare("DELETE FROM selection WHERE id = :selection_id");
  selStmt.bind(":selection_id", selectionId);

  if (!selStmt.exec()) {
    qDebug() << "failed to execute selection deletion" << selStmt.lastError().c_str();
    tx.rollback();
    return {};
  }

  tx.commit();

  return deletedOffers;
}

std::optional<PreferredClipboardOfferRecord>
ClipboardDatabase::findPreferredOffer(const QString &selectionId) {
  auto stmt = m_db.prepare(R"(
    SELECT o.id, o.encryption_type FROM data_offer o
    JOIN selection s ON s.id = o.selection_id
    WHERE o.mime_type = s.preferred_mime_type
    AND selection_id = :selection
  )");
  stmt.bind(":selection", selectionId);

  if (!stmt.step()) {
    qCritical() << "No match for default mime type";
    return {};
  }

  return PreferredClipboardOfferRecord{.id = stmt.columnQString(0),
                                       .encryption = static_cast<ClipboardEncryptionType>(stmt.columnInt(1))};
}

bool ClipboardDatabase::setPinned(const QString &id, bool pinned) {
  if (pinned) {
    auto stmt = m_db.prepare("UPDATE selection SET pinned_at = :epoch WHERE id = :id");
    stmt.bind(":id", id);
    stmt.bind(":epoch", static_cast<int64_t>(QDateTime::currentSecsSinceEpoch()));
    return stmt.exec();
  }

  auto stmt = m_db.prepare("UPDATE selection SET pinned_at = NULL WHERE id = :id");
  stmt.bind(":id", id);
  return stmt.exec();
}

bool ClipboardDatabase::insertSelection(const InsertSelectionPayload &payload) {
  auto stmt = m_db.prepare(R"(
    INSERT INTO selection (id, kind, offer_count, hash_md5, preferred_mime_type, source, created_at, updated_at)
    VALUES (:id, :kind, :offer_count, :hash_md5, :preferred_mime_type, :source, :epoch, :epoch)
    RETURNING id, created_at;
  )");
  stmt.bind(":id", payload.id);
  stmt.bind(":kind", static_cast<int>(payload.kind));
  stmt.bind(":offer_count", payload.offerCount);
  stmt.bind(":hash_md5", payload.hash);
  stmt.bind(":preferred_mime_type", payload.preferredMimeType);
  stmt.bind(":epoch", static_cast<int64_t>(QDateTime::currentSecsSinceEpoch()));
  stmt.bind(":source", payload.source);

  if (!stmt.exec()) {
    qCritical() << "Failed to insert selection" << stmt.lastError().c_str();
    return false;
  }

  return true;
}

bool ClipboardDatabase::transaction(const TxHandle &handle) {
  auto tx = m_db.transaction();

  if (handle(this)) { return tx.commit(); }

  tx.rollback();
  return false;
}

bool ClipboardDatabase::tryBubbleUpSelection(const QString &idLike) {
  auto stmt = m_db.prepare("UPDATE selection SET updated_at = :updated_at WHERE hash_md5 = :id OR id = :id");
  stmt.bind(":id", idLike);
  stmt.bind(":updated_at", static_cast<int64_t>(QDateTime::currentSecsSinceEpoch()));

  if (!stmt.exec()) { qCritical() << "Failed to execute clipboard update"; }

  return m_db.changes() > 0;
}

bool ClipboardDatabase::indexSelectionContent(const QString &selectionId, const QString &content) {
  auto stmt = m_db.prepare(R"(
    INSERT INTO selection_fts (selection_id, content) VALUES (:selection_id, :content);
  )");
  stmt.bind(":selection_id", selectionId);
  stmt.bind(":content", content);

  if (!stmt.exec()) {
    qCritical() << "failed to index text" << stmt.lastError().c_str();
    return false;
  }

  return true;
}

void ClipboardDatabase::runMigrations() {
  MigrationManager manager(m_db, "clipboard");

  manager.runMigrations();
}

bool ClipboardDatabase::insertOffer(const InsertClipboardOfferPayload &payload) {
  auto stmt = m_db.prepare(R"(
    INSERT INTO data_offer (id, selection_id, mime_type, text_preview, content_hash_md5, encryption_type, size, kind, url_host)
    VALUES (:id, :selection_id, :mime_type, :text_preview, :content_hash_md5, :encryption, :size, :kind, :url_host)
  )");
  stmt.bind(":id", payload.id);
  stmt.bind(":selection_id", payload.selectionId);
  stmt.bind(":mime_type", payload.mimeType);
  stmt.bind(":text_preview", payload.textPreview);
  stmt.bind(":content_hash_md5", payload.md5sum);
  stmt.bind(":encryption", static_cast<int>(payload.encryption));
  stmt.bind(":size", static_cast<int64_t>(payload.size));
  stmt.bind(":kind", static_cast<int>(payload.kind));
  stmt.bind(":url_host", payload.urlHost);

  if (!stmt.exec()) {
    qCritical() << "Failed to insert offer" << stmt.lastError().c_str();
    return false;
  }

  return true;
}

ClipboardDatabase::ClipboardDatabase() {
  auto result = db::Database::open(Omnicast::dataDir() / "clipboard.db");

  if (!result) {
    qCritical() << "Failed to open clipboard database:" << result.error().c_str();
    return;
  }

  m_db = std::move(*result);

  for (const auto &pragma : CLIPBOARD_PRAGMAS) {
    if (!m_db.exec(pragma)) { qCritical() << "Failed to execute pragma" << pragma; }
  }
}
