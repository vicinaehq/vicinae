#pragma once
#include "common.hpp"
#include <qsqldatabase.h>
#include <qvariant.h>

enum class ClipboardEncryptionType {
  None,
  Local,
};

struct PreferredClipboardOfferRecord {
  QString id;
  ClipboardEncryptionType encryption;
};

enum class ClipboardOfferKind { Unknown = 0, Text, Link, Image, File, Count /* a link ot a file */ };

struct InsertSelectionPayload {
  QString id;
  int offerCount;
  QString hash;
  QString preferredMimeType;
  ClipboardOfferKind kind;
  std::optional<QString> source;
};

struct InsertClipboardOfferPayload {
  QString id;
  QString selectionId;
  QString mimeType;
  QString textPreview;
  QString md5sum;
  ClipboardEncryptionType encryption;
  ClipboardOfferKind kind;
  quint64 size;
  std::optional<QString> urlHost;
};

struct InsertClipboardHistoryLine {
  QString mimeType;
  QString textPreview;
  QString md5sum;
};

struct ClipboardHistoryEntry {
  QString id;
  QString mimeType;
  QString textPreview;
  uint64_t pinnedAt;
  QString md5sum;
  uint64_t updatedAt;
  uint64_t size;
  ClipboardOfferKind kind;
  std::optional<QString> urlHost;
  ClipboardEncryptionType encryption;
};

struct ClipboardListSettings {
  QString query;
  std::optional<ClipboardOfferKind> kind;
};

struct ClipboardSelectionOfferRecord {
  QString id;
  QString mimeType;
  ClipboardEncryptionType encryption;
};

struct ClipboardSelectionRecord {
  std::vector<ClipboardSelectionOfferRecord> offers;
};

class ClipboardDatabase {

public:
  using TxHandle = std::function<bool(ClipboardDatabase &db)>;

  bool transaction(const TxHandle &handle);

  std::optional<ClipboardSelectionRecord> findSelection(const QString &id);

  PaginatedResponse<ClipboardHistoryEntry> query(int limit = 100, int offset = 0,
                                                 const ClipboardListSettings &opts = {}) const;

  bool removeAll();

  bool setKeywords(const QString &id, const QString &keywords);
  std::optional<QString> retrieveKeywords(const QString &id);

  bool setPinned(const QString &id, bool pinned);
  /**
   * Tries to take an existing selection and update its created_at date
   * to make it appear as new without duplicating it. Return whether a record
   * was updated or not.
   * The id can either be the selection id or the selection hash.
   */
  bool tryBubbleUpSelection(const QString &idLike);
  bool insertSelection(const InsertSelectionPayload &payload);
  bool insertOffer(const InsertClipboardOfferPayload &payload);
  bool indexSelectionContent(const QString &selectionId, const QString &content);
  /**
   * Remove the selection from the database and return the list of offers
   * that were deleted with it.
   */
  std::vector<QString> removeSelection(const QString &selectionId);
  std::optional<PreferredClipboardOfferRecord> findPreferredOffer(const QString &selectionId);

  /**
   * Apply new migrations if any. If no new migration is available this is a no-op.
   */
  void runMigrations();

  ClipboardDatabase();
  ~ClipboardDatabase();

private:
  QSqlDatabase m_db;
};
