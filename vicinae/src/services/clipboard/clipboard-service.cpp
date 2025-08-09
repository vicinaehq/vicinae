#include <QClipboard>
#include "services/clipboard/clipboard-service.hpp"
#include "common.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include <expected>
#include "crypto.hpp"
#include <openssl/rand.h>
#include <qbuffer.h>
#include <qfuturewatcher.h>
#include <qt6keychain/keychain.h>
#include "services/clipboard/clipboard-server.hpp"
#include "services/clipboard/clipboard-server-factory.hpp"
#include <algorithm>
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
#include <quuid.h>
#include <variant>

namespace fs = std::filesystem;

static const QString KEYCHAIN_ENCRYPTION_KEY_NAME = "clipboard-data-key";

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

  auto readJob = new ReadPasswordJob(Omnicast::APP_ID);

  readJob->setKey(KEYCHAIN_ENCRYPTION_KEY_NAME);
  readJob->start();

  connect(readJob, &ReadPasswordJob::finished, this, [this, readJob, promise](Job *job) {
    if (readJob->error() == QKeychain::NoError) {
      promise->addResult(readJob->binaryData());
      promise->finish();
      return;
    }

    auto writeJob = new QKeychain::WritePasswordJob(Omnicast::APP_ID);
    auto keyData = Crypto::AES256GCM::generateKey();

    writeJob->setKey(KEYCHAIN_ENCRYPTION_KEY_NAME);
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

QFuture<PaginatedResponse<ClipboardHistoryEntry>>
ClipboardService::listAll(int limit, int offset, const ClipboardListSettings &opts) const {
  QPromise<PaginatedResponse<ClipboardHistoryEntry>> promise;
  auto future = promise.future();

  QThreadPool::globalInstance()->start(
      [promise = std::move(promise), opts, limit, offset, dbPath = db.databaseName()]() mutable {
        auto conId = Crypto::UUID::v4();
        auto db = QSqlDatabase::addDatabase("QSQLITE", conId);

        db.setDatabaseName(dbPath);

        if (!db.open()) {
          qDebug() << "Failed to open database for listall query" << db.lastError();
          promise.addResult({});
          promise.finish();
          return;
        }
        {

          QSqlQuery query(db);

          if (!query.exec("SELECT COUNT(*) FROM selection;") || !query.next()) {
            promise.addResult({});
            promise.finish();
            return;
          }

          PaginatedResponse<ClipboardHistoryEntry> response;

          response.totalCount = query.value(0).toInt();
          response.totalPages = ceil(static_cast<double>(response.totalCount) / limit);
          response.currentPage = ceil(static_cast<double>(offset) / limit);
          response.data.reserve(limit);

          QString queryString = R"(
	  	SELECT
			selection.id, o.mime_type, o.text_preview, pinned_at, o.content_hash_md5, created_at, o.size
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
            promise.addResult({});
            promise.finish();
            return;
          }

          while (query.next()) {
            auto sum = query.value(4).toString();

            response.data.push_back(ClipboardHistoryEntry{
                .id = query.value(0).toString(),
                .mimeType = query.value(1).toString(),
                .textPreview = query.value(2).toString(),
                .pinnedAt = query.value(3).toULongLong(),
                .md5sum = sum,
                .createdAt = query.value(5).toULongLong(),
                .size = query.value(6).toULongLong(),
            });
          }
          timer.time("clipboard db query listAll");

          promise.addResult(response);
          promise.finish();
        }

        QSqlDatabase::removeDatabase(conId);
      });

  return future;
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

bool ClipboardService::removeSelection(const QString &selectionId) {
  if (!db.transaction()) { return false; }

  QSqlQuery query(db);

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
    db.rollback();
    return false;
  }

  std::vector<fs::path> pendingDeletions;

  while (query.next()) {
    auto id = query.value(0).toString();
    // we will delete only if transaction is successfully commited
    pendingDeletions.emplace_back(m_dataDir / id.toStdString());
  }

  query.prepare("DELETE FROM selection WHERE id = :selection_id");
  query.bindValue(":selection_id", selectionId);

  if (!query.exec()) {
    qDebug() << "failed to execute selecton deletion" << query.lastError();
    db.rollback();
    return false;
  }

  db.commit();

  for (const auto &path : pendingDeletions)
    fs::remove(path);

  return true;
}

QByteArray ClipboardService::decryptOffer(const QByteArray &data, ClipboardEncryptionType enc) const {
  switch (enc) {
  case ClipboardEncryptionType::None:
    return data;
  case ClipboardEncryptionType::Local: {
    if (!m_localEncryptionKey) {
      qCritical() << "No local encryption key available for decryption";
      return {};
    }

    qDebug() << "decrypt from local";

    return Crypto::AES256GCM::decrypt(data, *m_localEncryptionKey);
  }
  default:
    break;
  }

  return {};
}

QByteArray ClipboardService::decryptMainSelectionOffer(const QString &selectionId) const {
  Timer timer;
  QSqlQuery query(db);

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
  ClipboardEncryptionType encryption = parseEncryptionType(query.value(1).toString());
  fs::path path = m_dataDir / id.toStdString();
  QFile file(path);

  if (!file.open(QIODevice::ReadOnly)) {
    qCritical() << "Failed to open file at" << path;
    return {};
  }

  return decryptOffer(file.readAll(), encryption);
}

QByteArray ClipboardService::computeSelectionHash(const ClipboardSelection &selection) const {
  QCryptographicHash hash(QCryptographicHash::Md5);

  for (const auto &offer : selection.offers) {
    hash.addData(QCryptographicHash::hash(offer.data, QCryptographicHash::Md5));
  }

  return hash.result();
}

bool ClipboardService::isClearSelection(const ClipboardSelection &selection) const {
  return std::ranges::fold_left(selection.offers, 0,
                                [](size_t acc, auto &&item) { return acc + item.data.size(); }) == 0;
}

void ClipboardService::saveSelection(const ClipboardSelection &selection) {
  if (!m_monitoring || !m_isEncryptionReady) return;

  ClipboardHistoryEntry insertedEntry;

  if (isClearSelection(selection)) {
    qDebug() << "skipped clipboard clear";
    return;
  }

  bool isConcealed = std::ranges::any_of(
      selection.offers, [](auto &&offer) { return offer.mimeType == Clipboard::CONCEALED_MIME_TYPE; });

  if (isConcealed) {
    qDebug() << "Ignoring concealed selection";
    return;
  }

  auto selectionHash = QString::fromUtf8(computeSelectionHash(selection).toHex());

  if (!db.transaction()) {
    qDebug() << "Failed to create transaction" << db.lastError();
    return;
  }

  QSqlQuery query(db);

  query.prepare("UPDATE selection SET created_at = unixepoch() WHERE hash_md5 = :hash");
  query.addBindValue(selectionHash);

  if (!query.exec()) {
    qCritical() << "Failed to execute clipboard update";
    db.rollback();
    return;
  }

  if (query.numRowsAffected() > 0) {
    if (!db.commit()) { qCritical() << "Failed to commit transaction" << db.lastError(); }
    return;
  }

  std::string preferredMimeType = getSelectionPreferredMimeType(selection);
  std::vector<InsertClipboardHistoryLine> dbOffers;

  query.prepare(R"(
  	INSERT INTO selection (id, offer_count, hash_md5, preferred_mime_type, source)
	VALUES (:id, :offer_count, :hash_md5, :preferred_mime_type, :source)
	RETURNING id, created_at;
  )");
  query.bindValue(":id", Crypto::UUID::v4());
  query.bindValue(":offer_count", static_cast<uint>(selection.offers.size()));
  query.bindValue(":hash_md5", selectionHash);
  query.bindValue(":preferred_mime_type", preferredMimeType.c_str());

  // TODO: use window manager integration to figure out what's the currently focused window
  query.bindValue(":source", {});

  if (!query.exec() || !query.next()) {
    qDebug() << "Failed to insert selection" << query.lastError();
    db.rollback();
    return;
  }

  QString selectionId = query.value(0).toString();
  auto createdAt = query.value(1).toULongLong();

  for (const auto &offer : selection.offers) {
    QString textPreview;

    if (offer.mimeType.starts_with("text/")) {
      textPreview = createTextPreview(offer.data, 50);
      query.prepare(R"(
		INSERT INTO selection_fts (selection_id, content) VALUES (:selection_id, :content);
	)");
      query.bindValue(":selection_id", selectionId);
      query.bindValue(":content", offer.data);

      if (!query.exec()) {
        qDebug() << "failed to index text" << query.lastError();
        db.rollback();
        return;
      }

      // TODO: index text for selection
    }

    else if (offer.mimeType.starts_with("image/")) {
      QBuffer buffer;
      QImageReader reader(&buffer);

      buffer.setData(offer.data);

      if (auto size = reader.size(); size.isValid()) {
        textPreview = QString("Image (%1x%2)").arg(size.width()).arg(size.height());
      } else {
        textPreview = "Image";
      }
    } else {
      textPreview = "Unnamed";
    }

    auto md5sum = QCryptographicHash::hash(offer.data, QCryptographicHash::Md5).toHex();
    auto offerId = Crypto::UUID::v4();
    ClipboardEncryptionType encryption = ClipboardEncryptionType::None;

    if (m_localEncryptionKey) encryption = ClipboardEncryptionType::Local;

    query.prepare(R"(
		INSERT INTO data_offer (id, selection_id, mime_type, text_preview, content_hash_md5, encryption_type, size)
		VALUES (:id, :selection_id, :mime_type, :text_preview, :content_hash_md5, :encryption, :size)
  	)");
    query.bindValue(":id", offerId);
    query.bindValue(":selection_id", selectionId);
    query.bindValue(":mime_type", offer.mimeType.c_str());
    query.bindValue(":text_preview", textPreview);
    query.bindValue(":content_hash_md5", md5sum);
    query.bindValue(":encryption", stringifyEncryptionType(encryption));
    query.bindValue(":size", offer.data.size());

    if (!query.exec()) {
      db.rollback();
      return;
    }

    fs::path targetPath = m_dataDir / offerId.toStdString();
    QFile targetFile(targetPath);

    if (!targetFile.open(QIODevice::WriteOnly)) { continue; }

    if (m_localEncryptionKey) {
      targetFile.write(Crypto::AES256GCM::encrypt(offer.data, *m_localEncryptionKey));
    } else {
      targetFile.write(offer.data);
    }

    if (offer.mimeType == preferredMimeType) {
      insertedEntry.id = selectionId;
      insertedEntry.pinnedAt = 0;
      insertedEntry.createdAt = createdAt;
      insertedEntry.mimeType = offer.mimeType.c_str();
      insertedEntry.md5sum = md5sum;
      insertedEntry.textPreview = textPreview;
    }
  }

  db.commit();
  emit itemInserted(insertedEntry);
}

std::optional<ClipboardSelection> ClipboardService::retrieveSelectionById(const QString &id) {
  ClipboardSelection selection;
  QSqlQuery query(db);

  query.prepare("SELECT id, mime_type, encryption_type from data_offer where selection_id = :id");
  query.addBindValue(id);

  if (!query.exec()) {
    qCritical() << "Failed to retrieve selection for id" << id << query.lastError();
    return std::nullopt;
  }

  while (query.next()) {
    ClipboardDataOffer offer;
    QString id = query.value(0).toString();
    QString mimeType = query.value(1).toString();
    ClipboardEncryptionType encryption = parseEncryptionType(query.value(2).toString());
    fs::path path = m_dataDir / id.toStdString();
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) { continue; }

    offer.data = decryptOffer(file.readAll(), encryption);
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

  for (const auto &offer : selection.offers) {
    mimeData->setData(offer.mimeType.c_str(), offer.data);
  }

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
