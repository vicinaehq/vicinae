#include <QClipboard>
#include "services/clipboard/clipboard-service.hpp"
#include "common.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include <expected>
#include <openssl/rand.h>
#include <qfuturewatcher.h>
#include <qt6keychain/keychain.h>
#include "services/clipboard/clipboard-server.hpp"
#include "services/clipboard/clipboard-server-factory.hpp"
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <qbytearrayview.h>
#include "timer.hpp"
#include "utils/migration-manager/migration-manager.hpp"
#include "vicinae.hpp"
#include <qcontainerfwd.h>
#include <qcryptographichash.h>
#include <qdir.h>
#include <qevent.h>
#include <qimage.h>
#include <qimagereader.h>
#include <qlogging.h>
#include <qmimedata.h>
#include <qsqlquery.h>
#include <qtypes.h>
#include <sstream>
#include <variant>

namespace fs = std::filesystem;

static const char *retrieveSelectionByIdQuery =
    "SELECT mime_type, content_hash_md5 from data_offer where selection_id = :id";

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

bool ClipboardService::clear() {
  QApplication::clipboard()->clear();

  return true;
}

QFuture<ClipboardService::GetLocalEncryptionKeyResponse> ClipboardService::getLocalEncryptionKey() {
  using namespace QKeychain;

  auto promise = std::make_shared<QPromise<ClipboardService::GetLocalEncryptionKeyResponse>>();
  auto future = promise->future();
  const QString key = "clipboard-data-key";

  auto readJob = new ReadPasswordJob(Omnicast::APP_ID);

  readJob->setKey(key);
  readJob->start();

  connect(readJob, &ReadPasswordJob::finished, this, [this, readJob, promise, key](Job *job) {
    if (readJob->error() == QKeychain::NoError) {
      promise->addResult(readJob->binaryData());
      promise->finish();
      return;
    }

    auto writeJob = new QKeychain::WritePasswordJob(Omnicast::APP_ID);
    QByteArray keyData(32, 0);

    RAND_bytes(reinterpret_cast<unsigned char *>(keyData.data()), keyData.size());

    writeJob->setKey(key);
    writeJob->setBinaryData(keyData);
    writeJob->start();

    connect(writeJob, &WritePasswordJob::finished, this, [promise, keyData, writeJob]() {
      if (writeJob->error() == QKeychain::NoError) {
        promise->addResult(keyData);
        promise->finish();
        return;
      }

      qCritical() << "Failed to write encryption key to keychain" << writeJob->errorString();

      promise->addResult(std::unexpected(writeJob->error()));
      promise->finish();
    });
  });

  return future;
}

bool ClipboardService::copyContent(const Clipboard::Content &content, const Clipboard::CopyOptions options) {
  struct ContentVisitor {
    ClipboardService &service;
    const Clipboard::CopyOptions &options;

    bool operator()(const Clipboard::NoData &dummy) const {
      qWarning() << "attempt to copy NoData content";
      return false;
    }
    bool operator()(const Clipboard::Html &html) const { return service.copyHtml(html, options); }
    bool operator()(const Clipboard::File &file) const { return service.copyFile(file.path, options); }
    bool operator()(const Clipboard::Text &text) const { return service.copyText(text.text, options); }
    bool operator()(const ClipboardSelection &selection) const {
      return service.copySelection(selection, options);
    }

    ContentVisitor(ClipboardService &service, const Clipboard::CopyOptions &options)
        : service(service), options(options) {}
  };

  ContentVisitor visitor(*this, options);

  return std::visit(visitor, content);
}

bool ClipboardService::copyFile(const std::filesystem::path &path, const Clipboard::CopyOptions &options) {
  QMimeType mime = _mimeDb.mimeTypeForFile(path.c_str());
  std::ifstream ifs(path, std::ios_base::binary);

  if (!ifs) {
    qCritical() << "Cannot copy file to clipboard as it doesn't exist" << path;
    return false;
  }

  QMimeData *data = new QMimeData;
  std::stringstream ss;

  ss << ifs.rdbuf();
  const std::string &str = ss.str();
  data->setData(mime.name(), {str.data(), static_cast<qsizetype>(str.size())});

  return copyQMimeData(data, options);
}

void ClipboardService::setRecordAllOffers(bool value) {
  m_recordAllOffers = value;
  qCritical() << "set record all offers" << value;
}

void ClipboardService::setMonitoring(bool value) {
  m_monitoring = value;
  emit monitoringChanged(value);
}

bool ClipboardService::isServerRunning() const { return m_clipboardServer->isAlive(); }

bool ClipboardService::monitoring() const { return m_monitoring; }

bool ClipboardService::copyHtml(const Clipboard::Html &data, const Clipboard::CopyOptions &options) {
  auto mimeData = new QMimeData;

  mimeData->setData("text/html", data.html.toUtf8());

  if (auto text = data.text) mimeData->setData("text/plain", text->toUtf8());

  return copyQMimeData(mimeData, options);
}

bool ClipboardService::copyText(const QString &text, const Clipboard::CopyOptions &options) {
  QClipboard *clipboard = QApplication::clipboard();
  auto mimeData = new QMimeData;

  mimeData->setData("text/plain", text.toUtf8());

  if (options.concealed) mimeData->setData(Clipboard::CONCEALED_MIME_TYPE, "1");

  clipboard->setMimeData(mimeData);

  return true;
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

  if (!opts.query.isEmpty()) {}
  queryString += " GROUP BY selection.id ";

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
  std::filesystem::path path;
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

QByteArray ClipboardService::decryptMainSelectionOffer(int selectionId) const {
  QSqlQuery query(db);

  query.prepare(R"(
		SELECT content_hash_md5, encryption_type FROM data_offer o
		JOIN selection s ON s.id = o.selection_id
		WHERE o.mime_type = s.preferred_mime_type
		AND selection_id = :selection
	)");
  query.addBindValue(selectionId);

  if (!query.exec() || !query.next()) {
    qCritical() << "Failed to decrypt main selection offer" << query.lastError();
    return {};
  }

  QString md5sum = query.value(0).toString();
  ClipboardEncryptionType encryption = parseEncryptionType(query.value(1).toString());
  fs::path path = m_dataDir / md5sum.toStdString();
  QFile file(path);

  if (!file.open(QIODevice::ReadOnly)) {
    qCritical() << "Failed to open file at" << path;
    return {};
  }

  switch (encryption) {
  case ClipboardEncryptionType::None:
    return file.readAll();
  case ClipboardEncryptionType::Local:
    if (!m_localEncryptionKey) {
      qCritical() << "No local encryption key available for decryption";
      return {};
    }

    return decrypt(file.readAll(), *m_localEncryptionKey);
  default:
    break;
  }
}

QByteArray ClipboardService::decrypt(const QByteArray &encrypted, const QByteArray &key) const {
  if (key.size() != 32) {
    qWarning() << "Key must be exactly 32 bytes (256 bits)";
    return QByteArray();
  }

  if (encrypted.size() < 28) { // IV(12) + tag(16) = minimum 28 bytes
    qWarning() << "Encrypted data too short";
    return QByteArray();
  }

  // Extract components
  QByteArray iv = encrypted.left(12);
  QByteArray tag = encrypted.right(16);
  QByteArray ciphertext = encrypted.mid(12, encrypted.size() - 28);

  // Create cipher context
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (!ctx) return QByteArray();

  // Initialize decryption
  if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr,
                         reinterpret_cast<const unsigned char *>(key.constData()),
                         reinterpret_cast<const unsigned char *>(iv.constData())) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return QByteArray();
  }

  // Decrypt
  QByteArray plaintext(ciphertext.size(), 0);
  int len;
  if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char *>(plaintext.data()), &len,
                        reinterpret_cast<const unsigned char *>(ciphertext.constData()),
                        ciphertext.size()) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return QByteArray();
  }

  // Set expected tag
  if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16,
                          const_cast<void *>(reinterpret_cast<const void *>(tag.constData()))) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return QByteArray();
  }

  // Finalize and verify
  int plaintext_len = len;
  if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(plaintext.data()) + len, &len) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    qWarning() << "Authentication failed - data may be corrupted";
    return QByteArray();
  }

  plaintext_len += len;
  EVP_CIPHER_CTX_free(ctx);

  plaintext.resize(plaintext_len);
  return plaintext;
}

QByteArray ClipboardService::encrypt(const QByteArray &data, const QByteArray &key) const {
  QByteArray iv(12, 0);

  RAND_bytes(reinterpret_cast<unsigned char *>(iv.data()), iv.size());

  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr,
                     reinterpret_cast<const unsigned char *>(key.constData()),
                     reinterpret_cast<const unsigned char *>(iv.constData()));

  QByteArray ciphertext(data.size(), 0);
  int len;
  if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char *>(ciphertext.data()), &len,
                        reinterpret_cast<const unsigned char *>(data.constData()), data.size()) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return {};
  }

  int ciphertext_len = len;

  // Finalize encryption
  if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(ciphertext.data()) + len, &len) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return {};
  }
  ciphertext_len += len;

  // Get the authentication tag (16 bytes for GCM)
  QByteArray tag(16, 0);
  if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag.data()) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return {};
  }

  EVP_CIPHER_CTX_free(ctx);
  ciphertext.resize(ciphertext_len);

  // Return format: IV (12) + Ciphertext + Auth Tag (16)
  return iv + ciphertext + tag;
}

void ClipboardService::saveSelection(const ClipboardSelection &selection) {
  if (!m_monitoring || !m_isEncryptionReady) return;

  std::vector<TransformedSelection> transformedOffers;
  char buf[1 << 16];
  QCryptographicHash selectionHash(QCryptographicHash::Md5);
  uint64_t totalLength = 0;
  ClipboardHistoryEntry insertedEntry;

  transformedOffers.reserve(selection.offers.size());

  for (const auto &offer : selection.offers) {
    if (offer.mimeType == "omnicast/concealed") {
      qDebug() << "Ignoring selection because it offers mime type omnicast/concealed";
      return;
    }

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

    selectionHash.addData(offer.mimeType);
    selectionHash.addData(md5sum);
    selection.dataPath = targetFile.filePath();
    selection.md5sum = md5sum.toHex();
    selection.path = offer.path;
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

  if (!query.exec() || !query.next()) {
    qDebug() << "Failed to insert selection" << query.lastError();
    db.rollback();
    return;
  }

  int selectionId = query.value(0).toInt();
  auto createdAt = query.value(1).toULongLong();

  for (const auto &offer : transformedOffers) {
    ClipboardEncryptionType encryption;
    fs::path targetPath = m_dataDir / offer.md5sum.toStdString();

    if (m_localEncryptionKey) {
      QFile targetFile(targetPath);
      QFile file(offer.path);

      if (!file.open(QIODevice::ReadOnly)) { continue; }
      if (!targetFile.open(QIODevice::WriteOnly)) { continue; }

      QByteArray encrypted = encrypt(file.readAll(), *m_localEncryptionKey);

      targetFile.write(encrypted);
      encryption = ClipboardEncryptionType::Local;
    } else {
      encryption = ClipboardEncryptionType::None;
      fs::copy(offer.path, targetPath);
    }

    QString textPreview;

    if (offer.mimeType.startsWith("text/")) {
      QFile file(offer.path);

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
      QImageReader reader(offer.path.c_str());

      if (auto size = reader.size(); size.isValid()) {
        textPreview = QString("Image (%1x%2)").arg(size.width()).arg(size.height());
      } else {
        textPreview = "Image";
      }
    } else {
      textPreview = "Unnamed";
    }

    query.prepare(R"(
		INSERT INTO data_offer (selection_id, mime_type, text_preview, content_hash_md5, encryption_type)
		VALUES (:selection_id, :mime_type, :text_preview, :content_hash_md5, :encryption)
  	)");
    query.bindValue(":selection_id", selectionId);
    query.bindValue(":mime_type", offer.mimeType);
    query.bindValue(":text_preview", textPreview);
    query.bindValue(":content_hash_md5", offer.md5sum);
    query.bindValue(":encryption", stringifyEncryptionType(encryption));

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

std::optional<ClipboardSelection> ClipboardService::retrieveSelectionById(int id) {
  ClipboardSelection selection;

  m_retrieveSelectionByIdQuery.bindValue(":id", id);

  if (!m_retrieveSelectionByIdQuery.exec()) {
    qCritical() << "Failed to execute retrieveSelectionById query"
                << m_retrieveSelectionByIdQuery.lastError();
    return {};
  }

  while (m_retrieveSelectionByIdQuery.next()) {
    ClipboardDataOffer offer;
    QString mimeType = m_retrieveSelectionByIdQuery.value(0).toString();
    QString checksum = m_retrieveSelectionByIdQuery.value(1).toString();

    offer.path = _data_dir.filePath(checksum).toStdString();
    offer.mimeType = mimeType.toStdString();
    selection.offers.emplace_back(offer);
  }

  return selection;
}

bool ClipboardService::copyQMimeData(QMimeData *data, const Clipboard::CopyOptions &options) {
  QClipboard *clipboard = QApplication::clipboard();

  if (options.concealed) { data->setData(Clipboard::CONCEALED_MIME_TYPE, "1"); }

  clipboard->setMimeData(data);

  return true;
}

bool ClipboardService::copySelection(const ClipboardSelection &selection,
                                     const Clipboard::CopyOptions &options) {
  if (selection.offers.empty()) {
    qWarning() << "Not copying selection with no offers";
    return false;
  }

  QMimeData *mimeData = new QMimeData;
  auto appendOffer = [mimeData](const ClipboardDataOffer &offer) {
    if (!std::filesystem::exists(offer.path)) {
      qWarning() << "Cannot copy offer of type" << offer.mimeType << "contents do not exist at location"
                 << offer.path;
      return;
    }

    std::ifstream ifs(offer.path, std::ios_base::binary);
    std::stringstream ss;

    if (!ifs) {
      qWarning() << "Could not open" << offer.path << "for reading";
      return;
    }

    ss << ifs.rdbuf();
    QByteArray buf(ss.str().data(), ss.str().size());
    mimeData->setData(QString::fromStdString(offer.mimeType), buf);
  };

  std::ranges::for_each(selection.offers, appendOffer);

  return copyQMimeData(mimeData, options);
}

AbstractClipboardServer *ClipboardService::clipboardServer() const { return m_clipboardServer.get(); }

ClipboardService::ClipboardService(const std::filesystem::path &path)
    : db(QSqlDatabase::addDatabase("QSQLITE", "clipboard")), _path(path),
      _data_dir(_path.dir().filePath("clipboard-data")) {

  m_dataDir = path.parent_path() / "clipboard-data";

  m_clipboardServer =
      std::unique_ptr<AbstractClipboardServer>(ClipboardServerFactory().createFirstActivatable());

  // pragmas cannot be ran inside a transaction
  std::vector<QString> pragmas = {"PRAGMA journal_mode = WAL", "PRAGMA synchronous = normal",
                                  "PRAGMA journal_size_limit = 6144000"};

  db.setDatabaseName(path.c_str());

  if (!db.open()) { throw std::runtime_error("Failed to open clipboard db"); }

  for (const auto &pragma : pragmas) {
    QSqlQuery query(db);

    if (!query.exec(pragma)) { qCritical() << "Failed to run pragma" << pragma << query.lastError(); }
  }

  _data_dir.mkpath(_data_dir.path());

  QSqlQuery query(db);

  MigrationManager manager(db, "clipboard");

  manager.runMigrations();

  query.prepare(retrieveSelectionByIdQuery);

  if (!m_clipboardServer->start()) {
    qCritical() << "Failed to start clipboard server, clipboard monitoring will not work";
  }

  m_retrieveSelectionByIdQuery = QSqlQuery(db);
  m_retrieveSelectionByIdQuery.prepare(
      "SELECT mime_type, content_hash_md5 from data_offer where selection_id = :id");

  auto watcher = new QFutureWatcher<GetLocalEncryptionKeyResponse>;

  watcher->setFuture(getLocalEncryptionKey());

  connect(watcher, &QFutureWatcher<GetLocalEncryptionKeyResponse>::finished, this, [watcher, this]() {
    if (!watcher->isFinished()) return;

    auto res = watcher->result();

    if (res) {
      qCritical() << "got encryption key" << res.value().toStdString();
      m_localEncryptionKey = res.value();
    } else {
      res.error();
    }

    m_isEncryptionReady = true; // at that point, we know whether we can encrypt or not
  });

  connect(m_clipboardServer.get(), &AbstractClipboardServer::selection, this,
          &ClipboardService::saveSelection);
}
