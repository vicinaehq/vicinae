#include <QClipboard>
#include "clipboard-service.hpp"
#include <filesystem>
#include <format>
#include <qimagereader.h>
#include <qlogging.h>
#include <qmimedata.h>
#include <qsqlquery.h>
#include <qt6keychain/keychain.h>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include "clipboard-server-factory.hpp"
#include "crypto.hpp"
#include "services/clipboard/clipboard-db.hpp"

namespace fs = std::filesystem;

static const QString KEYCHAIN_ENCRYPTION_KEY_NAME = "clipboard-data-key";

bool ClipboardService::setPinned(const QString id, bool pinned) {
  if (!ClipboardDatabase().setPinned(id, pinned)) { return false; }

  emit selectionPinStatusChanged(id, pinned);

  return true;
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
  QFile file(path);

  if (!file.open(QIODevice::ReadOnly)) { return false; }

  QMimeData *data = new QMimeData;

  data->setData(mime.name(), file.readAll());

  return copyQMimeData(data, options);
}

void ClipboardService::setRecordAllOffers(bool value) { m_recordAllOffers = value; }

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
  return QtConcurrent::run(
      [opts, limit, offset]() { return ClipboardDatabase().listAll(limit, offset, opts); });
}

ClipboardOfferKind ClipboardService::getKind(const ClipboardDataOffer &offer) {
  if (offer.mimeType.starts_with("image/")) return ClipboardOfferKind::Image;
  if (offer.mimeType.starts_with("text/")) {
    if (offer.mimeType == "text/html") { return ClipboardOfferKind::Text; }
    auto url = QUrl::fromEncoded(offer.data, QUrl::StrictMode);
    if (url.isValid() && !url.scheme().isEmpty()) { return ClipboardOfferKind::Link; }

    return ClipboardOfferKind::Text;
  }

  // some of these can be text
  if (offer.mimeType.starts_with("application/")) {
    static auto applicationTexts =
        std::vector<std::string>{"json", "xml", "javascript", "sql"} |
        std::views::transform([](auto &&text) { return std::format("application/{}", text); });

    if (std::ranges::contains(applicationTexts, offer.mimeType)) return ClipboardOfferKind::Text;
  }

  return ClipboardOfferKind::Unknown;
}

std::string ClipboardService::getSelectionPreferredMimeType(const ClipboardSelection &selection) {
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
      } else if (offer.mimeType == "image/svg+xml") {
        mimePriority = SelectionPriority::ImageSvg;
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

bool ClipboardService::removeSelection(const QString &selectionId) {
  ClipboardDatabase cdb;

  for (const auto &offer : cdb.removeSelection(selectionId)) {
    fs::remove(m_dataDir / offer.toStdString());
  }

  emit selectionRemoved(selectionId);

  return true;
}

QByteArray ClipboardService::decryptOffer(const QByteArray &data, ClipboardEncryptionType enc) const {
  switch (enc) {
  case ClipboardEncryptionType::None:
    return data;
  case ClipboardEncryptionType::Local: {
    if (!m_localEncryptionKey) {
      qWarning() << "No local encryption key available for decryption";
      return {};
    }

    return Crypto::AES256GCM::decrypt(data, *m_localEncryptionKey);
  }
  default:
    break;
  }

  qWarning() << "unknown encryption kind" << static_cast<int>(enc);

  return {};
}

QByteArray ClipboardService::decryptMainSelectionOffer(const QString &selectionId) const {
  ClipboardDatabase cdb;

  auto offer = cdb.findPreferredOffer(selectionId);

  if (!offer) {
    qWarning() << "Can't find preferred offer for selection" << selectionId;
    return {};
  };

  fs::path path = m_dataDir / offer->id.toStdString();

  QFile file(path);

  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Failed to open file at" << path;
    return {};
  }

  auto data = file.readAll();

  return decryptOffer(data, offer->encryption);
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

QString ClipboardService::getOfferTextPreview(const ClipboardDataOffer &offer) {
  if (offer.mimeType.starts_with("text/")) { return offer.data.simplified().mid(0, 50); }

  if (offer.mimeType.starts_with("image/")) {
    QBuffer buffer;
    QImageReader reader(&buffer);

    buffer.setData(offer.data);

    if (auto size = reader.size(); size.isValid()) {
      return QString("Image (%1x%2)").arg(size.width()).arg(size.height());
    }

    return "Image";
  }

  return "Unnamed";
}

void ClipboardService::saveSelection(ClipboardSelection selection) {
  if (!m_monitoring || !m_isEncryptionReady) return;

  std::ranges::unique(selection.offers, [](auto &&a, auto &&b) { return a.mimeType == b.mimeType; });

  if (isClearSelection(selection)) { return; }

  bool isConcealed = std::ranges::any_of(
      selection.offers, [](auto &&offer) { return offer.mimeType == Clipboard::CONCEALED_MIME_TYPE; });

  if (isConcealed) {
    qDebug() << "Ignoring concealed selection";
    return;
  }

  auto selectionHash = QString::fromUtf8(computeSelectionHash(selection).toHex());
  std::string preferredMimeType = getSelectionPreferredMimeType(selection);

  // capturing this inside this lambda is safe because we only read data
  ClipboardHistoryEntry insertedEntry;
  ClipboardDatabase cdb;

  auto &preferredOffer =
      *std::ranges::find_if(selection.offers, [&](auto &&o) { return o.mimeType == preferredMimeType; });

  cdb.transaction([&](ClipboardDatabase &db) {
    // selection already exists, stop here
    if (db.tryBubbleUpSelection(selectionHash)) return true;

    QString selectionId = Crypto::UUID::v4();
    ClipboardOfferKind kind = getKind(preferredOffer);

    if (!db.insertSelection({.id = selectionId,
                             .offerCount = static_cast<int>(selection.offers.size()),
                             .hash = selectionHash,
                             .preferredMimeType = preferredMimeType.c_str(),
                             .kind = kind})) {
      qDebug() << "failed to insert selection";
      return false;
    }

    for (const auto &offer : selection.offers) {
      ClipboardOfferKind kind = getKind(offer);
      bool isIndexableText = kind == ClipboardOfferKind::Text || kind == ClipboardOfferKind::Link;
      QString textPreview = getOfferTextPreview(offer);

      if (isIndexableText) {
        if (!db.indexSelectionContent(selectionId, offer.data)) return false;
      }

      auto md5sum = QCryptographicHash::hash(offer.data, QCryptographicHash::Md5).toHex();
      auto offerId = Crypto::UUID::v4();
      ClipboardEncryptionType encryption = ClipboardEncryptionType::None;

      if (m_localEncryptionKey) encryption = ClipboardEncryptionType::Local;

      InsertClipboardOfferPayload dto{.id = offerId,
                                      .selectionId = selectionId,
                                      .mimeType = offer.mimeType.c_str(),
                                      .textPreview = textPreview,
                                      .md5sum = md5sum,
                                      .encryption = encryption,
                                      .size = static_cast<quint64>(offer.data.size())};

      if (kind == ClipboardOfferKind::Link) {
        auto url = QUrl::fromEncoded(offer.data, QUrl::StrictMode);

        if (url.scheme().startsWith("http")) { dto.urlHost = url.host(); }
      }

      db.insertOffer(dto);

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
        insertedEntry.updatedAt = {};
        insertedEntry.mimeType = offer.mimeType.c_str();
        insertedEntry.md5sum = md5sum;
        insertedEntry.textPreview = textPreview;
      }
    }

    return true;
  });

  emit itemInserted(insertedEntry);
}

std::optional<ClipboardSelection> ClipboardService::retrieveSelectionById(const QString &id) {
  ClipboardDatabase cdb;
  ClipboardSelection populatedSelection;
  const auto selection = cdb.findSelection(id);

  if (!selection) return std::nullopt;

  for (const auto &offer : selection->offers) {
    ClipboardDataOffer populatedOffer;
    fs::path path = m_dataDir / offer.id.toStdString();
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) { continue; }

    populatedOffer.data = decryptOffer(file.readAll(), offer.encryption);
    populatedOffer.mimeType = offer.mimeType.toStdString();
    populatedSelection.offers.emplace_back(populatedOffer);
  }

  return populatedSelection;
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

bool ClipboardService::removeAllSelections() {
  ClipboardDatabase db;

  if (!db.removeAll()) return false;

  fs::remove_all(m_dataDir);
  fs::create_directories(m_dataDir);

  emit allSelectionsRemoved();

  return true;
}

AbstractClipboardServer *ClipboardService::clipboardServer() const { return m_clipboardServer.get(); }

ClipboardService::ClipboardService(const std::filesystem::path &path) {
  m_dataDir = path.parent_path() / "clipboard-data";
  m_clipboardServer =
      std::unique_ptr<AbstractClipboardServer>(ClipboardServerFactory().createFirstActivatable());

  fs::create_directories(m_dataDir);

  if (!m_clipboardServer->start()) {
    qCritical() << "Failed to start clipboard server, clipboard monitoring will not work";
  }

  ClipboardDatabase().runMigrations();

  auto watcher = new QFutureWatcher<GetLocalEncryptionKeyResponse>;

  watcher->setFuture(getLocalEncryptionKey());

  connect(watcher, &QFutureWatcher<GetLocalEncryptionKeyResponse>::finished, this, [watcher, this]() {
    if (!watcher->isFinished()) return;

    auto res = watcher->result();

    if (res) { m_localEncryptionKey = res.value(); }
    m_isEncryptionReady = true; // at that point, we know whether we can encrypt or not
  });

  connect(m_clipboardServer.get(), &AbstractClipboardServer::selection, this,
          &ClipboardService::saveSelection);
}
