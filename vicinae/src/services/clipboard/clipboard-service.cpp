#include <QClipboard>
#include "clipboard-service.hpp"
#include <filesystem>
#include <numeric>
#include <qapplication.h>
#include "x11/x11-clipboard-server.hpp"
#include <qimagereader.h>
#include <qlogging.h>
#include <qmimedata.h>
#include <qnamespace.h>
#include <qsqlquery.h>
#include <qstringview.h>
#include <qt6keychain/keychain.h>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QBuffer>
#include <QImage>
#include "clipboard-server-factory.hpp"
#include "crypto.hpp"
#include "services/app-service/app-service.hpp"
#include "services/clipboard/clipboard-db.hpp"
#include "services/clipboard/clipboard-encrypter.hpp"
#include "services/clipboard/clipboard-server.hpp"
#include "services/clipboard/gnome/gnome-clipboard-server.hpp"
#include "utils.hpp"
#include "wlr/wlr-clipboard-server.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/window-manager.hpp"

namespace fs = std::filesystem;

/**
 * If any of these is found in a selection, we ignore the entire selection.
 */
static const std::set<QString> IGNORED_MIME_TYPES = {
    Clipboard::CONCEALED_MIME_TYPE,
    "x-kde-passwordManagerHint",
};

bool ClipboardService::setPinned(const QString id, bool pinned) {
  if (!ClipboardDatabase().setPinned(id, pinned)) { return false; }

  emit selectionPinStatusChanged(id, pinned);

  return true;
}

bool ClipboardService::clear() {
  QApplication::clipboard()->clear();

  return true;
}

bool ClipboardService::supportsMonitoring() const { return m_clipboardServer->id() != "dummy"; }

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
    bool operator()(const Clipboard::SelectionRecordHandle &handle) const {
      return service.copySelectionRecord(handle.id, options);
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
  Keyboard::Shortcut shortcut = Keyboard::Shortcut::osPaste();

  if (auto app = m_appDb.find(window->wmClass())) {
    if (app->isTerminalEmulator()) { shortcut = shortcut.shifted(); }
  }

  m_wm.provider()->sendShortcutSync(*window, shortcut);

  return true;
}

bool ClipboardService::copyFile(const std::filesystem::path &path, const Clipboard::CopyOptions &options) {
  QMimeData *data = new QMimeData;

  // copying files should normally copy a link to the file, not the file itself
  // This is what text/uri-list is used for. On Windows or other systems we might have
  // to do something else, I'm not sure.
  data->setData("text/uri-list", QString("file://%1").arg(path.c_str()).toUtf8());

  return copyQMimeData(data, options);
}

void ClipboardService::setRecordAllOffers(bool value) { m_recordAllOffers = value; }

void ClipboardService::setEncryption(bool value) {
  m_encrypter.reset();

  if (value) {
    m_encrypter = std::make_unique<ClipboardEncrypter>();
    m_encrypter->loadKey();
  }
}

bool ClipboardService::isEncryptionReady() const { return m_encrypter.get(); }

void ClipboardService::setMonitoring(bool value) {
  if (m_monitoring == value) return;

  if (value) {
    qInfo() << "Starting clipboard server" << m_clipboardServer->id();
    if (m_clipboardServer->start()) {
      qInfo() << "Clipboard server" << m_clipboardServer->id() << "started successfully.";
    } else {
      qWarning() << "Failed to start clipboard server" << m_clipboardServer->id();
    }
  } else {
    qInfo() << "Stopping clipboard server" << m_clipboardServer->id();
    if (m_clipboardServer->stop()) {
      qInfo() << "Clipboard server" << m_clipboardServer->id() << "stopped successfully.";
    } else {
      qWarning() << "Failed to stop clipboard server" << m_clipboardServer->id();
    }
  }

  m_monitoring = value;
  emit monitoringChanged(value);
}

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
  if (offer.mimeType == "text/uri-list") {
    QString text = offer.data;
    auto uris = text.split("\r\n", Qt::SkipEmptyParts);
    if (uris.size() == 1 && QUrl(uris.front()).isLocalFile()) return ClipboardOfferKind::File;
    return ClipboardOfferKind::Text;
  }

  if (offer.mimeType.startsWith("image/")) { return ClipboardOfferKind::Image; }
  if (offer.mimeType == "text/html") { return ClipboardOfferKind::Text; }

  if (Utils::isTextMimeType(offer.mimeType)) {
    auto url = QUrl::fromEncoded(offer.data, QUrl::StrictMode);

    if (url.isLocalFile()) { return ClipboardOfferKind::File; }
    if (!url.scheme().isEmpty()) { return ClipboardOfferKind::Link; }

    return ClipboardOfferKind::Text;
  }

  return ClipboardOfferKind::Unknown;
}

QString ClipboardService::getSelectionPreferredMimeType(const ClipboardSelection &selection) {
  static const std::vector<QString> plainTextMimeTypes = {
      "text/uri-list", "text/plain;charset=utf-8", "text/plain", "UTF8_STRING", "STRING", "TEXT",
      "COMPOUND_TEXT"};

  for (const auto &mime : plainTextMimeTypes) {
    auto it = std::ranges::find_if(
        selection.offers, [&](const auto &offer) { return offer.mimeType == mime && !offer.data.isEmpty(); });
    if (it != selection.offers.end()) return it->mimeType;
  }

  auto imageIt = std::ranges::find_if(selection.offers, [](const auto &offer) {
    return offer.mimeType.startsWith("image/") && !offer.data.isEmpty();
  });
  if (imageIt != selection.offers.end()) return imageIt->mimeType;

  auto htmlIt = std::ranges::find_if(selection.offers, [](const auto &offer) {
    return offer.mimeType == "text/html" && !offer.data.isEmpty();
  });
  if (htmlIt != selection.offers.end()) return htmlIt->mimeType;

  auto fallbackIt = std::ranges::find_if(selection.offers, [](const auto &offer) {
    return !offer.mimeType.startsWith("text/_moz_html") && !offer.data.isEmpty();
  });
  if (fallbackIt != selection.offers.end()) return fallbackIt->mimeType;

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

tl::expected<QByteArray, ClipboardService::OfferDecryptionError>
ClipboardService::decryptOffer(const QByteArray &data, ClipboardEncryptionType type) const {
  switch (type) {
  case ClipboardEncryptionType::Local: {
    if (!m_encrypter) { return tl::unexpected(OfferDecryptionError::DecryptionRequired); }
    auto decryption = m_encrypter->decrypt(data);
    if (!decryption) { return tl::unexpected(OfferDecryptionError::DecryptionFailed); }
    return decryption.value();
  }
  default:
    return data;
  }
}

tl::expected<QByteArray, ClipboardService::OfferDecryptionError>
ClipboardService::getMainOfferData(const QString &selectionId) const {
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

  return decryptOffer(file.readAll(), offer->encryption);
}

QByteArray ClipboardService::computeSelectionHash(const ClipboardSelection &selection) const {
  QCryptographicHash hash(QCryptographicHash::Md5);

  for (const auto &offer : selection.offers) {
    hash.addData(QCryptographicHash::hash(offer.data, QCryptographicHash::Md5));
  }

  return hash.result();
}

bool ClipboardService::isClearSelection(const ClipboardSelection &selection) const {
  return std::accumulate(selection.offers.begin(), selection.offers.end(), 0,
                         [](size_t acc, auto &&item) { return acc + item.data.size(); }) == 0;
}

QString ClipboardService::getOfferTextPreview(const ClipboardDataOffer &offer) {
  switch (getKind(offer)) {
  case ClipboardOfferKind::Text:
  case ClipboardOfferKind::Link:
  case ClipboardOfferKind::File:
    return offer.data.simplified().mid(0, 50);
  case ClipboardOfferKind::Image: {
    QBuffer buffer;
    QImageReader reader(&buffer);

    buffer.setData(offer.data);
    if (auto size = reader.size(); size.isValid()) {
      return QString("Image (%1x%2)").arg(size.width()).arg(size.height());
    }
    return "Image";
  }
  default:
    return "Unknown";
  }
}

std::optional<QString> ClipboardService::retrieveKeywords(const QString &id) {
  return ClipboardDatabase().retrieveKeywords(id);
}

bool ClipboardService::setKeywords(const QString &id, const QString &keywords) {
  return ClipboardDatabase().setKeywords(id, keywords);
}

bool ClipboardService::isConcealedSelection(const ClipboardSelection &selection) {
  return std::ranges::any_of(selection.offers,
                             [](auto &&offer) { return IGNORED_MIME_TYPES.contains(offer.mimeType); });
}

ClipboardSelection &ClipboardService::sanitizeSelection(ClipboardSelection &selection) {
  std::ranges::sort(selection.offers, [](auto &&a, auto &&b) {
    return std::ranges::lexicographical_compare(a.mimeType, b.mimeType);
  });
  std::ranges::unique(selection.offers, [](auto &&a, auto &&b) { return a.mimeType == b.mimeType; });

  return selection;
}

void ClipboardService::saveSelection(ClipboardSelection selection) {
  if (!m_monitoring) return;

  sanitizeSelection(selection);

  if (isConcealedSelection(selection)) {
    qDebug() << "Ignoring concealed selection";
    return;
  }

  if (isClearSelection(selection)) {
    qDebug() << "Ignored clipboard clear selection";
    return;
  }

  QString preferredMimeType = getSelectionPreferredMimeType(selection);
  ClipboardHistoryEntry insertedEntry;
  ClipboardDatabase cdb;
  auto preferredOfferIt =
      std::ranges::find_if(selection.offers, [&](auto &&o) { return o.mimeType == preferredMimeType; });

  if (preferredOfferIt == selection.offers.end()) {
    qCritical() << "preferredOfferIt is invalid, this should not be possible!";
    return;
  }

  auto preferredKind = getKind(*preferredOfferIt);

  if (preferredKind == ClipboardOfferKind::Unknown) {
    qDebug() << "Ignoring selection with primary offer of unknown kind" << preferredMimeType;
    return;
  }

  auto selectionHash = QCryptographicHash::hash(preferredOfferIt->data, QCryptographicHash::Md5).toHex();

  if (preferredKind == ClipboardOfferKind::Text && preferredOfferIt->data.trimmed().isEmpty()) {
    qDebug() << "Ignore empty text for preferred offer of type" << preferredMimeType;
    return;
  }

  cdb.transaction([&](ClipboardDatabase &db) {
    if (db.tryBubbleUpSelection(selectionHash)) {
      qDebug() << "bubbled up selection hash" << selectionHash;
      return true;
    }

    qDebug() << "process selectionHash" << selectionHash;

    QString selectionId = Crypto::UUID::v4();

    if (!db.insertSelection({.id = selectionId,
                             .offerCount = static_cast<int>(selection.offers.size()),
                             .hash = selectionHash,
                             .preferredMimeType = preferredMimeType,
                             .kind = preferredKind,
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

      if (m_encrypter) encryption = ClipboardEncryptionType::Local;

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

      if (m_encrypter) {
        if (auto encrypted = m_encrypter->encrypt(offer.data)) {
          targetFile.write(encrypted.value());
        } else {
          qWarning() << "Failed to encrypt clipboard selection";
          return false;
        }
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

    auto data = decryptOffer(file.readAll(), offer.encryption);

    if (!data) return {};

    populatedOffer.data = data.value();
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

bool ClipboardService::copySelectionRecord(const QString &id, const Clipboard::CopyOptions &options) {
  auto selection = retrieveSelectionById(id);

  if (!selection) {
    qWarning() << "copySelectionRecord: could not get selection for ID" << id;
    return false;
  }

  ClipboardDatabase db;

  if (!db.tryBubbleUpSelection(id)) {
    qWarning() << "Failed to bubble up selection with id" << id;
    return false;
  }

  // we don't want subscribers to block before the actual copy happens
  QMetaObject::invokeMethod(this, [this]() { emit selectionUpdated(); }, Qt::QueuedConnection);

  return copySelection(*selection, options);
}

QString ClipboardService::readText() { return QApplication::clipboard()->text(); }

Clipboard::ReadContent ClipboardService::readContent() {
  Clipboard::ReadContent content;
  const QMimeData *mimeData = QApplication::clipboard()->mimeData();

  if (!mimeData) return content;

  if (mimeData->hasUrls()) {
    for (const auto &url : mimeData->urls()) {
      if (url.isLocalFile()) {
        content.file = url.toLocalFile();
        break;
      }
    }
  }

  if (mimeData->hasHtml()) { content.html = mimeData->html(); }
  if (mimeData->hasText()) { content.text = mimeData->text(); }

  return content;
}

bool ClipboardService::removeAllSelections() {
  ClipboardDatabase db;

  if (!db.removeAll()) {
    qWarning() << "Failed to remove all clipboard selections";
    return false;
  }

  fs::remove_all(m_dataDir);
  fs::create_directories(m_dataDir);

  emit allSelectionsRemoved();

  return true;
}

AbstractClipboardServer *ClipboardService::clipboardServer() const { return m_clipboardServer.get(); }

ClipboardService::ClipboardService(const std::filesystem::path &path, WindowManager &wm, AppService &appDb)
    : m_wm(wm), m_appDb(appDb) {
  m_dataDir = path.parent_path() / "clipboard-data";

  {
    ClipboardServerFactory factory;

    factory.registerServer<GnomeClipboardServer>();
    factory.registerServer<WlrClipboardServer>();
    factory.registerServer<X11ClipboardServer>();
    m_clipboardServer = factory.createFirstActivatable();
    qInfo() << "Activated clipboard server" << m_clipboardServer->id();
  }

  fs::create_directories(m_dataDir);
  ClipboardDatabase().runMigrations();

  connect(m_clipboardServer.get(), &AbstractClipboardServer::selectionAdded, this,
          &ClipboardService::saveSelection);
}
