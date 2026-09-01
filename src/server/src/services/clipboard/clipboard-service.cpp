#include <QClipboard>
#include "clipboard-service.hpp"
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <numeric>
#include <QGuiApplication>
#include <qfuturewatcher.h>
#include <qstandardpaths.h>
#include <qtconcurrentrun.h>
#include <qthreadpool.h>
#include "common/clipboard-formats.hpp"
#include "common/types.hpp"
#ifdef Q_OS_LINUX
#include "x11/x11-clipboard-server.hpp"
#endif
#include <qclipboard.h>
#include <qimagereader.h>
#include <qlogging.h>
#include <qmimedata.h>
#include <qnamespace.h>
#include <qstringview.h>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QBuffer>
#include <QImage>
#include "clipboard-server-factory.hpp"
#include <quuid.h>
#include "fuzzy/fuzzy-searchable.hpp"
#include "services/clipboard/clipboard-db.hpp"
#include "services/clipboard/selection-mime-data.hpp"
#include "services/clipboard/clipboard-encrypter.hpp"
#include "services/clipboard/clipboard-mime.hpp"
#include "services/clipboard/clipboard-server.hpp"
#include "utils.hpp"
#ifdef Q_OS_LINUX
#ifdef Q_OS_LINUX
#include "services/clipboard/gnome/gnome-clipboard-server.hpp"
#include "data-control/data-control-clipboard-server.hpp"
#endif
#endif
#ifdef Q_OS_MACOS
#include "macos/macos-clipboard-server.hpp"
#endif
#ifdef Q_OS_WIN
#include "windows/windows-clipboard-server.hpp"
#endif

namespace fs = std::filesystem;

bool ClipboardService::setPinned(const QString &id, bool pinned) {
  if (!openDatabase().setPinned(id, pinned)) { return false; }

  emit selectionPinStatusChanged(id, pinned);

  return true;
}

bool ClipboardService::clear() {
  QGuiApplication::clipboard()->clear();
  return true;
}

bool ClipboardService::supportsMonitoring() const { return m_clipboardServer->id() != "dummy"; }

bool ClipboardService::copyContent(Clipboard::Content content, const Clipboard::CopyOptions &options) {
  const auto visitor = overloads{
      [&](const Clipboard::Html &html) { return copyHtml(html, options); },
      [&](const Clipboard::File &file) { return copyFile(file.path, options); },
      [&](const Clipboard::Urls &urls) { return copyUrls(urls.values, options); },
      [&](const Clipboard::Text &text) { return copyText(text.text, options); },
      [&](ClipboardSelection &&selection) { return copySelection(std::move(selection), options); },
      [&](const Clipboard::SelectionRecordHandle &handle) { return copySelectionRecord(handle.id, options); },
      [](const auto &dummy) {
        qWarning() << "attempt to copy NoData content";
        return false;
      },
  };

  return std::visit(visitor, content);
}

bool ClipboardService::copyFile(const std::filesystem::path &path, const Clipboard::CopyOptions &options) {
  QMimeData *data = new QMimeData;

  data->setUrls({QUrl::fromLocalFile(QString::fromStdString(path.string()))});

  return copyQMimeData(data, options);
}

bool ClipboardService::copyUrls(const std::vector<QUrl> &urls, const Clipboard::CopyOptions &options) {
  auto data = Clipboard::mimeDataForContent(Clipboard::Urls{urls});

  return copyQMimeData(data.release(), options);
}

void ClipboardService::setRecordAllOffers(bool value) { m_recordAllOffers = value; }

void ClipboardService::setEncryptionKey(std::optional<db::EncryptionKey> key) {
  if (key) {
    m_encrypter = std::make_unique<ClipboardEncrypter>(
        QByteArray(reinterpret_cast<const char *>(key->data()), key->size()));
  } else {
    m_encrypter.reset();
  }
}

bool ClipboardService::isEncryptionReady() const { return m_encrypter.get(); }

void ClipboardService::setIgnorePasswords(bool value) { m_ignorePasswords = value; }

void ClipboardService::setHistoryEvictionThreshold(std::optional<std::chrono::seconds> threshold,
                                                   bool preserveTaggedSelections) {
  if (threshold == m_evictionThreshold && preserveTaggedSelections == m_preserveTaggedSelections) return;

  m_evictionThreshold = threshold;
  m_preserveTaggedSelections = preserveTaggedSelections;
  constexpr auto MISCONFIGURATION_GRACE_DELAY = std::chrono::seconds(60);

  m_historyEvictionTimer.stop();

  if (m_evictionThreshold) m_historyEvictionTimer.start(MISCONFIGURATION_GRACE_DELAY);
}

void ClipboardService::armEvictionTimer(std::optional<int64_t> oldestTimestamp) {
  using namespace std::chrono;
  using namespace std::chrono_literals;

  constexpr auto maxDelay = duration_cast<seconds>(6h);

  if (!m_evictionThreshold || !oldestTimestamp) return;

  const auto now = duration_cast<seconds>(system_clock::now().time_since_epoch());
  const auto delay = std::clamp(seconds(*oldestTimestamp) + *m_evictionThreshold - now + 1s, 1s, maxDelay);

  m_historyEvictionTimer.start(duration_cast<milliseconds>(delay));
}

void ClipboardService::pauseEviction() { m_evictionPaused = true; }

void ClipboardService::resumeEviction() {
  m_evictionPaused = false;
  if (std::exchange(m_evictionDeferred, false)) runEvictionPass();
}

void ClipboardService::runEvictionPass() {
  if (!m_evictionThreshold) return;

  if (m_evictionPaused) {
    m_evictionDeferred = true;
    return;
  }

  // this can be expensive, so we run it in a separate thread
  QThreadPool::globalInstance()->start(
      [this, t = *m_evictionThreshold, preserve = m_preserveTaggedSelections]() {
        auto db = openDatabase();
        const auto evictedIds = db.evictOlderThan(t, preserve);
        const auto oldest = db.oldestEvictableTimestamp(preserve);
        std::error_code ec{};
        std::size_t evictedCount = 0;

        for (const auto &evicted : evictedIds) {
          fs::path path = m_dataDir / evicted.toStdString();
          if (fs::remove(path, ec)) {
            ++evictedCount;
          } else {
            qWarning() << "failed to remove clipboard offer at" << path;
          }
        }

        if (evictedCount > 0) qInfo() << "evicted" << evictedCount << "clipboard offers";

        QMetaObject::invokeMethod(this, [this, oldest]() { armEvictionTimer(oldest); });
      });
}

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
  auto mimeData = new QMimeData;

  mimeData->setData("text/plain", text.toUtf8());
  mimeData->setData("text/plain;charset=utf-8", text.toUtf8());

  return copyQMimeData(mimeData, options);
}

void ClipboardService::scheduleClipboardRestore(int delayMs) {
  if (!m_lastSelection || m_lastSelection->offers.empty()) return;

  m_restoreTimer.stop();
  m_restoreTimer.setSingleShot(true);
  m_restoreTimer.setInterval(delayMs);
  m_restoreTimer.disconnect();
  connect(&m_restoreTimer, &QTimer::timeout, this, &ClipboardService::restoreClipboard);
  m_restoreTimer.start();
}

static void rerankByPreviewMatch(std::vector<ClipboardHistoryEntry> &entries, const QString &queryText) {
  auto const utf8 = queryText.toUtf8();
  fuzzy::Query const query{std::string_view(utf8.constData(), static_cast<size_t>(utf8.size()))};

  if (query.empty()) return;

  std::vector<Scored<ClipboardHistoryEntry>> scored;
  scored.reserve(entries.size());

  for (auto &entry : entries) {
    auto const preview = entry.textPreview.toUtf8();
    auto const match = fuzzy::scoreWeighted(
        {{std::string_view(preview.constData(), static_cast<size_t>(preview.size())), 1.0}}, query);
    scored.push_back({.data = std::move(entry), .score = match.accepted() ? match.score : -1});
  }

  std::ranges::stable_sort(scored, std::greater{});
  std::ranges::transform(scored, entries.begin(), [](auto &s) { return std::move(s.data); });
}

QFuture<PaginatedResponse<ClipboardHistoryEntry>>
ClipboardService::listAll(int limit, int offset, const ClipboardListSettings &opts) const {
  return QtConcurrent::run([db = m_readDb, opts, limit, offset]() {
    auto response = db->query(limit, offset, opts);
    rerankByPreviewMatch(response.data, opts.query);
    return response;
  });
}

ClipboardOfferKind ClipboardService::getKind(const ClipboardDataOffer &offer) {
  if (offer.mimeType == "text/uri-list") {
    QString const text = offer.data;
    auto uris = text.split("\r\n", Qt::SkipEmptyParts);
    auto isLocalFile = [](const QString &uri) { return QUrl(uri).isLocalFile(); };
    if (!uris.isEmpty() && std::ranges::all_of(uris, isLocalFile)) return ClipboardOfferKind::File;
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
      "text/plain;charset=utf-8", "text/plain", "UTF8_STRING", "STRING", "TEXT", "COMPOUND_TEXT"};

  auto uriIt = std::ranges::find_if(selection.offers, [](const auto &offer) {
    return offer.mimeType == "text/uri-list" && !offer.data.isEmpty();
  });
  if (uriIt != selection.offers.end() && getKind(*uriIt) == ClipboardOfferKind::File) {
    return uriIt->mimeType;
  }

  auto imageIt = std::ranges::find_if(selection.offers, [](const auto &offer) {
    return offer.mimeType.startsWith("image/") && !offer.data.isEmpty();
  });

  auto isRemoteUrl = [](const QByteArray &data) {
    auto url = QUrl::fromEncoded(data.trimmed(), QUrl::StrictMode);
    return url.scheme().length() > 1 && !url.isLocalFile();
  };

  for (const auto &mime : plainTextMimeTypes) {
    auto it = std::ranges::find_if(
        selection.offers, [&](const auto &offer) { return offer.mimeType == mime && !offer.data.isEmpty(); });
    if (it == selection.offers.end()) continue;
    if (imageIt != selection.offers.end() && isRemoteUrl(it->data)) break;
    return it->mimeType;
  }

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
  auto cdb = openDatabase();

  for (const auto &offer : cdb.removeSelection(selectionId)) {
    fs::remove(m_dataDir / offer.toStdString());
  }

  emit selectionRemoved(selectionId);

  return true;
}

std::expected<QByteArray, ClipboardService::OfferDecryptionError>
ClipboardService::decryptOffer(const QByteArray &data, ClipboardEncryptionType type) const {
  switch (type) {
  case ClipboardEncryptionType::Local: {
    if (!m_encrypter) { return std::unexpected(OfferDecryptionError::DecryptionRequired); }
    auto decryption = m_encrypter->decrypt(data);
    if (!decryption) { return std::unexpected(OfferDecryptionError::DecryptionFailed); }
    return decryption.value();
  }
  default:
    return data;
  }
}

std::expected<QByteArray, ClipboardService::OfferDecryptionError>
ClipboardService::getMainOfferData(const QString &selectionId) const {
  auto cdb = openDatabase();

  auto offer = cdb.findPreferredOffer(selectionId);

  if (!offer) {
    qWarning() << "Can't find preferred offer for selection" << selectionId;
    return std::unexpected(OfferDecryptionError::DataUnavailable);
  };

  fs::path const path = m_dataDir / offer->id.toStdString();

  QFile file(path);

  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Failed to open file at" << path;
    return std::unexpected(OfferDecryptionError::DataUnavailable);
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
    QImageReader const reader(&buffer);

    buffer.setData(offer.data);
    if (auto size = reader.size(); size.isValid()) {
      return tr("Image (%1x%2)").arg(size.width()).arg(size.height());
    }
    return tr("Image");
  }
  default:
    return tr("Unknown");
  }
}

std::optional<QString> ClipboardService::retrieveKeywords(const QString &id) {
  return openDatabase().retrieveKeywords(id);
}

bool ClipboardService::setKeywords(const QString &id, const QString &keywords) {
  if (!openDatabase().setKeywords(id, keywords)) return false;

  emit selectionKeywordsChanged(id, keywords);

  return true;
}

ClipboardSelection &ClipboardService::sanitizeSelection(ClipboardSelection &selection) {
  {
    std::ranges::sort(selection.offers, [](auto &&a, auto &&b) {
      return std::ranges::lexicographical_compare(a.mimeType, b.mimeType);
    });
    const auto [first, last] =
        std::ranges::unique(selection.offers, [](auto &&a, auto &&b) { return a.mimeType == b.mimeType; });

    selection.offers.erase(first, last);
  }

  {
    // we never want to index raw image data, as we cannot propose it back without paying
    // a significant price for it.
    auto it = std::ranges::find_if(selection.offers,
                                   [](auto &&offer) { return offer.mimeType == Clipboard::QT_IMAGE_DATA; });

    if (it != selection.offers.end()) selection.offers.erase(it);
  }

  return selection;
}

void ClipboardService::saveSelection(ClipboardSelection selection) {
  if (!m_monitoring) return;

  m_lastSelection = selection;

  sanitizeSelection(selection);

  qInfo() << "Received new clipboard selection with" << selection.offers.size()
          << "offers (password=" << selection.isPassword << ")";

  for (const auto &offer : selection.offers) {
    qInfo().nospace() << offer.mimeType << " (size=" << formatSize(offer.data.size()) << ")";
  }

  if (m_ignorePasswords && selection.isPassword) {
    qInfo() << "Ignored password clipboard selection";
    return;
  }

  if (isClearSelection(selection)) {
    qInfo() << "Ignored clipboard clear selection";
    return;
  }

  QString preferredMimeType = getSelectionPreferredMimeType(selection);
  auto preferredOfferIt =
      std::ranges::find_if(selection.offers, [&](auto &&o) { return o.mimeType == preferredMimeType; });

  if (preferredOfferIt == selection.offers.end()) {
    qCritical() << "preferredOfferIt is invalid, this should not be possible!";
    return;
  }

  auto preferredKind = getKind(*preferredOfferIt);

  if (preferredKind == ClipboardOfferKind::Unknown) {
    qWarning() << "Ignoring selection with primary offer of unknown kind" << preferredMimeType;
    return;
  }

  auto selectionHash = QCryptographicHash::hash(preferredOfferIt->data, QCryptographicHash::Md5).toHex();

  if (preferredKind == ClipboardOfferKind::Text && preferredOfferIt->data.trimmed().isEmpty()) {
    qInfo() << "Ignored text selection with empty text";
    return;
  }

  // we wait synchronously instead of queuing, in practice this will almost never happen,
  // so adding queuing infrastructure seems unnecessary here.
  if (m_indexingSelection.isRunning()) m_indexingSelection.waitForFinished();

  m_indexingSelection.setFuture(
      QtConcurrent::run([this, selection = std::move(selection), selectionHash, preferredKind,
                         preferredMimeType]() -> std::expected<ClipboardHistoryEntry, QString> {
        ClipboardHistoryEntry insertedEntry;
        auto cdb = openDatabase();
        const bool ok = cdb.transaction([&](ClipboardDatabase *db) {
          if (db->tryBubbleUpSelection(selectionHash)) {
            qInfo() << "A similar clipboard selection is already indexed: moving it on top of the history";
            return true;
          }

          QString const selectionId = QUuid::createUuid().toString(QUuid::WithoutBraces);

          if (!db->insertSelection({.id = selectionId,
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
            ClipboardOfferKind const kind = getKind(offer);
            bool const isIndexableText = kind == ClipboardOfferKind::Text || kind == ClipboardOfferKind::Link;
            QString const textPreview = getOfferTextPreview(offer);

            if (isIndexableText && !offer.data.isEmpty()) {
              if (!db->indexSelectionContent(selectionId, offer.data)) {
                qWarning() << "Failed to index selection content for offer" << offer.mimeType;
                return false;
              }
            }

            auto md5sum = QCryptographicHash::hash(offer.data, QCryptographicHash::Md5).toHex();
            auto offerId = QUuid::createUuid().toString(QUuid::WithoutBraces);
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

            if (!db->insertOffer(dto)) {
              qWarning() << "Failed to insert offer" << offer.mimeType;
              return false;
            }

            fs::path const targetPath = m_dataDir / offerId.toStdString();
            QFile targetFile(targetPath);

            if (!targetFile.open(QIODevice::WriteOnly)) { continue; }

            if (m_encrypter) {
              if (auto encrypted = m_encrypter->encrypt(offer.data)) {
                targetFile.write(encrypted.value());
                ClipboardHistoryEntry insertedEntry;
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

        if (!ok) return std::unexpected(QStringLiteral("Failed to insert selection"));

        return insertedEntry;
      }));
}

std::optional<ClipboardSelection> ClipboardService::retrieveSelectionById(const QString &id) {
  auto cdb = openDatabase();
  ClipboardSelection populatedSelection;
  const auto selection = cdb.findSelection(id);

  if (!selection) return std::nullopt;

  for (const auto &offer : selection->offers) {
    ClipboardDataOffer populatedOffer;
    fs::path const path = m_dataDir / offer.id.toStdString();
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) { continue; }

    auto data = decryptOffer(file.readAll(), offer.encryption);

    if (!data) return {};

    populatedOffer.data = data.value();
    populatedOffer.mimeType = offer.mimeType;
    populatedSelection.offers.emplace_back(populatedOffer);
  }

  populatedSelection.sourceApp = selection->source;
  return populatedSelection;
}

bool ClipboardService::copyQMimeData(QMimeData *data, const Clipboard::CopyOptions &options) {
  return m_clipboardServer->setClipboardContent(data, options);
}

void ClipboardService::restoreClipboard() {
  if (!m_lastSelection || m_lastSelection->offers.empty()) return;

  auto *data = new QMimeData;
  for (const auto &offer : m_lastSelection->offers) {
    data->setData(offer.mimeType, offer.data);
  }

  // Restore is transient so we don't re-index a selection that was already on the clipboard.
  m_clipboardServer->setClipboardContent(data, {.transient = true, .sourceApp = m_lastSelection->sourceApp});
  m_lastSelection.reset();
}

std::unique_ptr<QMimeData>
ClipboardService::mimeDataFromSelection(const ClipboardSelection &selection) const {
  QMimeData *mimeData = new QMimeData;

  for (auto &offer : selection.offers) {
    if (offer.mimeType != Clipboard::URI_LIST && Utils::isTextMimeType(offer.mimeType)) {
      mimeData->setText(QString::fromUtf8(offer.data));
    } else {
      mimeData->setData(offer.mimeType, offer.data);
    }
  }

  return std::unique_ptr<QMimeData>{mimeData};
}

std::unique_ptr<QMimeData>
ClipboardService::dragMimeDataForSelection(const ClipboardSelection &selection) const {
  return std::make_unique<DragAndDropSelectionMimeData>(selection);
}

bool ClipboardService::copySelection(const ClipboardSelection &selection,
                                     const Clipboard::CopyOptions &options) {
  if (selection.offers.empty()) {
    qWarning() << "Not copying selection with no offers";
    return false;
  }

  auto enrichedOptions = options;

  if (!enrichedOptions.sourceApp) enrichedOptions.sourceApp = selection.sourceApp;

  auto mimeData = mimeDataFromSelection(std::move(selection));

  return copyQMimeData(mimeData.release(), enrichedOptions);
}

bool ClipboardService::copySelectionRecord(const QString &id, const Clipboard::CopyOptions &options) {
  auto selection = retrieveSelectionById(id);

  if (!selection) {
    qWarning() << "copySelectionRecord: could not get selection for ID" << id;
    return false;
  }

  auto db = openDatabase();

  if (!db.tryBubbleUpSelection(id)) {
    qWarning() << "Failed to bubble up selection with id" << id;
    return false;
  }

  // we don't want subscribers to block before the actual copy happens
  QMetaObject::invokeMethod(this, [this]() { emit selectionUpdated(); }, Qt::QueuedConnection);

  return copySelection(*std::move(selection), options);
}

QString ClipboardService::readText() { return QGuiApplication::clipboard()->text(); }

Clipboard::ReadContent ClipboardService::readContent() {
  Clipboard::ReadContent content;
  const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData();

  if (!mimeData) return content;

  if (mimeData->hasUrls()) {
    const auto urls = mimeData->urls();
    content.urls.assign(urls.begin(), urls.end());
  }

  if (mimeData->hasHtml()) { content.html = mimeData->html(); }
  if (mimeData->hasText()) { content.text = mimeData->text(); }

  return content;
}

bool ClipboardService::removeAllSelections() {
  auto db = openDatabase();
  const auto removedIds = db.removeAll(m_preserveTaggedSelections);

  if (!removedIds) {
    qWarning() << "Failed to remove all clipboard selections";
    return false;
  }

  if (m_preserveTaggedSelections) {
    std::error_code ec{};

    for (const auto &id : *removedIds) {
      fs::remove(m_dataDir / id.toStdString(), ec);
    }
  } else {
    fs::remove_all(m_dataDir);
    fs::create_directories(m_dataDir);
  }

  emit allSelectionsRemoved();

  return true;
}

AbstractClipboardServer *ClipboardService::clipboardServer() const { return m_clipboardServer.get(); }

ClipboardService::ClipboardService(const std::filesystem::path &path, std::optional<db::EncryptionKey> key)
    : m_dbKey(key) {
  m_dataDir = path.parent_path() / "clipboard-data";

  {
    ClipboardServerFactory factory;

#ifdef Q_OS_LINUX
    factory.registerServer<GnomeClipboardServer>();
    factory.registerServer<DataControlClipboardServer>();
    factory.registerServer<X11ClipboardServer>();
#endif
#ifdef Q_OS_MACOS
    factory.registerServer<MacosClipboardServer>();
#endif
#ifdef Q_OS_WIN
    factory.registerServer<WindowsClipboardServer>();
#endif
    m_clipboardServer = factory.createFirstActivatable();
    qInfo() << "Activated clipboard server" << m_clipboardServer->id();
  }

  fs::create_directories(m_dataDir);
  openDatabase().runMigrations();
  m_readDb = std::make_shared<ClipboardDatabase>(m_dbKey);

  connect(m_clipboardServer.get(), &AbstractClipboardServer::selectionAdded, this,
          &ClipboardService::saveSelection);
  m_historyEvictionTimer.setSingleShot(true);
  m_historyEvictionTimer.setTimerType(Qt::VeryCoarseTimer);
  connect(&m_historyEvictionTimer, &QTimer::timeout, this, &ClipboardService::runEvictionPass);
  connect(&m_indexingSelection, &decltype(m_indexingSelection)::finished, this, [this]() {
    if (m_indexingSelection.isCanceled()) return;
    if (auto result = m_indexingSelection.result()) { emit itemInserted(*result); }

    if (m_evictionThreshold && !m_historyEvictionTimer.isActive()) {
      const auto now = std::chrono::duration_cast<std::chrono::seconds>(
          std::chrono::system_clock::now().time_since_epoch());
      armEvictionTimer(now.count());
    }
  });
}
