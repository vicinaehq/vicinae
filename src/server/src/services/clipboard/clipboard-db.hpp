#pragma once
#include <cstdint>
#include <QString>
#include <optional>
#include <functional>
#include <vector>
#include "common/paginated.hpp"
#include "db/database.hpp"

enum class ClipboardEncryptionType : std::uint8_t {
  None,
  Local,
};

struct PreferredClipboardOfferRecord {
  QString id;
  ClipboardEncryptionType encryption;
};

enum class ClipboardOfferKind : std::uint8_t {
  Unknown = 0,
  Text,
  Link,
  Image,
  File,
  Count /* a link ot a file */
};

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
  using TxHandle = std::function<bool(ClipboardDatabase *db)>;

  bool transaction(const TxHandle &handle);

  std::optional<ClipboardSelectionRecord> findSelection(const QString &id);

  PaginatedResponse<ClipboardHistoryEntry> query(int limit = 100, int offset = 0,
                                                 const ClipboardListSettings &opts = {}) const;

  bool removeAll();

  bool setKeywords(const QString &id, const QString &keywords);
  std::optional<QString> retrieveKeywords(const QString &id);

  bool setPinned(const QString &id, bool pinned);
  bool tryBubbleUpSelection(const QString &idLike);
  bool insertSelection(const InsertSelectionPayload &payload);
  bool insertOffer(const InsertClipboardOfferPayload &payload);
  bool indexSelectionContent(const QString &selectionId, const QString &content);
  std::vector<QString> removeSelection(const QString &selectionId);
  std::optional<PreferredClipboardOfferRecord> findPreferredOffer(const QString &selectionId);

  void runMigrations();

  explicit ClipboardDatabase(std::optional<db::EncryptionKey> key);
  ~ClipboardDatabase() = default;

private:
  db::Database m_db;
};
