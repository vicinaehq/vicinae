#include <QClipboard>
#include "clipboard-service.hpp"
#include <filesystem>
#include <qimagereader.h>
#include <qlogging.h>
#include <qmimedata.h>
#include <qsqlquery.h>
#include <qt6keychain/keychain.h>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include "clipboard-server-factory.hpp"
#include "crypto.hpp"
#include "services/app-service/app-service.hpp"
#include "services/clipboard/clipboard-db.hpp"
#include "wlr/wlr-clipboard-server.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/window-manager.hpp"

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

bool ClipboardService::pasteContent(const Clipboard::Content &content, const Clipboard::CopyOptions options) {
  if (!copyContent(content, options)) return false;

  if (!m_wm.provider()->supportsInputForwarding()) {
    qWarning()
        << "pasteContent called but no window manager capable of input forwarding is running. Falling i"
           "back to regular clipboard copy";
    return false;
  }

  auto window = m_wm.getFocusedWindow();
  KeyboardShortcut shortcut = KeyboardShortcut::paste();

  if (auto app = m_appDb.find(window->wmClass())) {
    if (app->isTerminalEmulator()) { shortcut = KeyboardShortcut::shiftPaste(); }
  }

  m_wm.provider()->sendShortcutSync(*window, shortcut);

  return true;
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
  if (offer.mimeType.startsWith("image/")) return ClipboardOfferKind::Image;
  if (offer.mimeType.startsWith("text/")) {
    if (offer.mimeType == "text/html") { return ClipboardOfferKind::Text; }
    auto url = QUrl::fromEncoded(offer.data, QUrl::StrictMode);
    if (url.isValid() && !url.scheme().isEmpty()) { return ClipboardOfferKind::Link; }

    return ClipboardOfferKind::Text;
  }

  // some of these can be text
  if (offer.mimeType.startsWith("application/")) {
    static auto applicationTexts =
        std::vector<QString>{"json", "xml", "javascript", "sql"} |
        std::views::transform([](auto &&text) { return QString("application/%1").arg(text); });

    if (std::ranges::contains(applicationTexts, offer.mimeType)) return ClipboardOfferKind::Text;
  }

  return ClipboardOfferKind::Unknown;
}

QString ClipboardService::getSelectionPreferredMimeType(const ClipboardSelection &selection) {
  static const std::vector<QString> plainTextMimeTypes = {
    "text/plain",
    "text/plain;charset=utf-8",
    "UTF8_STRING",
    "STRING",
    "TEXT",
    "COMPOUND_TEXT"
  };

  // 1. Prefer plain text (non-empty)
  for (const auto &mime : plainTextMimeTypes) {
    auto it = std::ranges::find_if(selection.offers, [&](const auto &offer) {
      return offer.mimeType == mime && !offer.data.isEmpty();
    });
    if (it != selection.offers.end()) return it->mimeType;
  }

  // 2. Then image types (non-empty)
  auto imageIt = std::ranges::find_if(selection.offers, [](const auto &offer) {
    return offer.mimeType.startsWith("image/") && !offer.data.isEmpty();
  });
  if (imageIt != selection.offers.end()) return imageIt->mimeType;

  // 3. Then HTML (non-empty)
  auto htmlIt = std::ranges::find_if(selection.offers, [](const auto &offer) {
    return offer.mimeType == "text/html" && !offer.data.isEmpty();
  });
  if (htmlIt != selection.offers.end()) return htmlIt->mimeType;

  // 4. Otherwise, fallback to first non-Firefox/Zen custom type (non-empty)
  auto fallbackIt = std::ranges::find_if(selection.offers, [](const auto &offer) {
    return !offer.mimeType.startsWith("text/_moz_html") && !offer.data.isEmpty();
  });
  if (fallbackIt != selection.offers.end()) return fallbackIt->mimeType;

  // 5. If nothing else, fallback to first offer (even if empty)
  if (!selection.offers.empty()) return selection.offers.front().mimeType;

  return {};
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
  if (offer.mimeType.startsWith("text/")) { return offer.data.simplified().mid(0, 50); }

  if (offer.mimeType.startsWith("image/")) {
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

std::optional<QString> ClipboardService::retrieveKeywords(const QString &id) {
  return ClipboardDatabase().retrieveKeywords(id);
}

bool ClipboardService::setKeywords(const QString &id, const QString &keywords) {
  return ClipboardDatabase().setKeywords(id, keywords);
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
  QString preferredMimeType = getSelectionPreferredMimeType(selection);

  ClipboardHistoryEntry insertedEntry;
  ClipboardDatabase cdb;

  auto preferredOfferIt =
      std::ranges::find_if(selection.offers, [&](auto &&o) { return o.mimeType == preferredMimeType; });

  cdb.transaction([&](ClipboardDatabase &db) {
    if (db.tryBubbleUpSelection(selectionHash)) return true;

    QString selectionId = Crypto::UUID::v4();
    ClipboardOfferKind kind = getKind(*preferredOfferIt);

    if (!db.insertSelection({.id = selectionId,
                             .offerCount = static_cast<int>(selection.offers.size()),
                             .hash = selectionHash,
                             .preferredMimeType = preferredMimeType,
                             .kind = kind,
                             .source = selection.sourceApp})) {
      qWarning() << "failed to insert selection";
      return false;
    }

    // Index all offers, including empty ones
    for (const auto &offer : selection.offers) {
      ClipboardOfferKind kind = getKind(offer);
      bool isIndexableText = kind == ClipboardOfferKind::Text || kind == ClipboardOfferKind::Link;
      QString textPreview = getOfferTextPreview(offer);

      if (isIndexableText && !offer.data.isEmpty()) {
        if (!db.indexSelectionContent(selectionId, offer.data)) return false;
      }

      auto md5sum = QCryptographicHash::hash(offer.data, QCryptographicHash::Md5).toHex();
      auto offerId = Crypto::UUID::v4();
      ClipboardEncryptionType encryption = ClipboardEncryptionType::None;

      if (m_localEncryptionKey) encryption = ClipboardEncryptionType::Local;

      InsertClipboardOfferPayload dto{
          .id = offerId,
          .selectionId = selectionId,
          .mimeType = offer.mimeType,
          .textPreview = textPreview,
          .md5sum = md5sum,
          .encryption = encryption,
          .size = static_cast<quint64>(offer.data.size()),
      };

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

      // Set the insertedEntry for the preferred offer
      if (offer.mimeType == preferredMimeType) {
        insertedEntry.id = selectionId;
        insertedEntry.pinnedAt = 0;
        insertedEntry.updatedAt = {};
        insertedEntry.mimeType = offer.mimeType;
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
    populatedOffer.mimeType = offer.mimeType;
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
    mimeData->setData(offer.mimeType, offer.data);
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

ClipboardService::ClipboardService(const std::filesystem::path &path, WindowManager &wm, AppService &app)
    : m_wm(wm), m_appDb(app) {
  m_dataDir = path.parent_path() / "clipboard-data";

  {
    ClipboardServerFactory factory;

    factory.registerServer<WlrClipboardServer>();
    m_clipboardServer = factory.createFirstActivatable();

    qInfo() << "Activated clipboard server" << m_clipboardServer->id();
  }

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

  connect(m_clipboardServer.get(), &AbstractClipboardServer::selectionAdded, this,
          &ClipboardService::saveSelection);
}
