#include "wlr-clipboard-server.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "utils/environment.hpp"
#include "wlr-data-control-unstable-v1-client-protocol.h"
#include <QApplication>
#include <QSocketNotifier>
#include <QGuiApplication>
#include <QClipboard>
#include <QMimeData>
#include <QBuffer>
#include <QCryptographicHash>
#include <cstdint>
#include <qlogging.h>
#include <algorithm>
#include <array>
#include <qmimedata.h>
#include <set>
#include <unistd.h>
#include <fcntl.h>

static void data_control_offer_offer(void *data, zwlr_data_control_offer_v1 *offer, const char *mime_type) {
  WlrClipboardServer *server = static_cast<WlrClipboardServer *>(data);
  server->onOfferMimeType(offer, mime_type);
}

static const struct zwlr_data_control_offer_v1_listener offer_listener = {
    .offer = data_control_offer_offer,
};

static void data_control_device_data_offer(void *data, zwlr_data_control_device_v1 *device,
                                           zwlr_data_control_offer_v1 *offer) {
  WlrClipboardServer *server = static_cast<WlrClipboardServer *>(data);
  server->onDataOffer(offer);
}

static void data_control_device_selection(void *data, zwlr_data_control_device_v1 *device,
                                          zwlr_data_control_offer_v1 *offer) {
  WlrClipboardServer *server = static_cast<WlrClipboardServer *>(data);
  server->onSelectionChanged(offer);
}

static void data_control_device_finished(void *data, zwlr_data_control_device_v1 *device) {}

static void data_control_device_primary_selection(void *data, zwlr_data_control_device_v1 *device,
                                                  zwlr_data_control_offer_v1 *offer) {}

static const struct zwlr_data_control_device_v1_listener device_listener = {
    .data_offer = data_control_device_data_offer,
    .selection = data_control_device_selection,
    .finished = data_control_device_finished,
    .primary_selection = data_control_device_primary_selection,
};

static void registry_handle_global(void *data, wl_registry *registry, uint32_t name, const char *interface,
                                   uint32_t version) {
  WlrClipboardServer *server = static_cast<WlrClipboardServer *>(data);

  if (strcmp(interface, zwlr_data_control_manager_v1_interface.name) == 0) {
    auto manager = static_cast<zwlr_data_control_manager_v1 *>(
        wl_registry_bind(registry, name, &zwlr_data_control_manager_v1_interface, std::min(version, 2u)));
    server->setDataControlManager(manager);
  }
}

static void registry_handle_global_remove(void *data, wl_registry *registry, uint32_t name) {}

static const struct wl_registry_listener registry_listener = {
    .global = registry_handle_global,
    .global_remove = registry_handle_global_remove,
};

WlrClipboardServer::WlrClipboardServer() {}

WlrClipboardServer::~WlrClipboardServer() { stop(); }

bool WlrClipboardServer::isActivatable() const {
  return Environment::isWaylandSession() && !Environment::isGnomeEnvironment();
}

void WlrClipboardServer::setDataControlManager(zwlr_data_control_manager_v1 *manager) { m_manager = manager; }

QString WlrClipboardServer::id() const { return "wlr-clipboard"; }

int WlrClipboardServer::activationPriority() const { return 1; }

bool WlrClipboardServer::isAlive() const { return m_started && m_manager && m_device; }

bool WlrClipboardServer::start() {
  qInfo() << "Starting WlrClipboardServer";

  m_display = wl_display_connect(nullptr);
  if (!m_display) {
    qCritical() << "Failed to connect to Wayland display";
    emit statusChanged("Could not connect to Wayland display");
    return false;
  }

  m_registry = wl_display_get_registry(m_display);

  static wl_seat *foundSeat = nullptr;
  foundSeat = nullptr;

  auto seatListener = wl_registry_listener{
      .global =
          [](void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
            if (strcmp(interface, wl_seat_interface.name) == 0) {
              foundSeat = static_cast<wl_seat *>(wl_registry_bind(registry, name, &wl_seat_interface, 1));
            }
            registry_handle_global(data, registry, name, interface, version);
          },
      .global_remove = registry_handle_global_remove,
  };

  wl_registry_add_listener(m_registry, &seatListener, this);
  wl_display_roundtrip(m_display);

  m_seat = foundSeat;
  if (!m_seat) {
    qCritical() << "Failed to get Wayland seat";
    emit statusChanged("Could not get Wayland seat");
    wl_display_disconnect(m_display);
    m_display = nullptr;
    return false;
  }

  m_registry = wl_display_get_registry(m_display);
  wl_registry_add_listener(m_registry, &registry_listener, this);
  wl_display_roundtrip(m_display);

  if (!m_manager) {
    QString error = "zwlr_data_control_manager_v1 is not exposed by your Wayland compositor.";
    QString shortError = "Compositor doesn't support wlr-data-control";

    if (Environment::isCosmicDesktop()) {
      error += " You seem to be using the COSMIC compositor: Note that the wlr-data-control protocol is not "
               "exposed by the COSMIC compositor for "
               "security reasons. In order to enable it, you need to set COSMIC_DATA_CONTROL_ENABLED=1 "
               "before starting COSMIC.";
      shortError += " COSMIC needs to be launched with COSMIC_DATA_CONTROL_ENABLED=1";
    }

    qCritical() << error;
    emit statusChanged(shortError);
    return false;
  }

  m_device = zwlr_data_control_manager_v1_get_data_device(m_manager, m_seat);
  zwlr_data_control_device_v1_add_listener(m_device, &device_listener, this);

  wl_display_roundtrip(m_display);

  int displayFd = wl_display_get_fd(m_display);
  m_displayNotifier = new QSocketNotifier(displayFd, QSocketNotifier::Read, this);
  connect(m_displayNotifier, &QSocketNotifier::activated, this, [this]() {
    if (wl_display_prepare_read(m_display) == 0) {
      wl_display_read_events(m_display);
      wl_display_dispatch_pending(m_display);
    } else {
      wl_display_dispatch_pending(m_display);
    }
  });

  auto clip = QApplication::clipboard();
  connect(clip, &QClipboard::dataChanged, this, &WlrClipboardServer::qtClipboardChanged);

  m_started = true;
  qInfo() << "WlrClipboardServer started successfully";

  return true;
}

bool WlrClipboardServer::stop() {
  qInfo() << "Stopping WlrClipboardServer";

  cancelPendingReads();

  auto clip = QApplication::clipboard();
  disconnect(clip, &QClipboard::dataChanged, this, &WlrClipboardServer::qtClipboardChanged);

  if (m_displayNotifier) {
    m_displayNotifier->setEnabled(false);
    m_displayNotifier->deleteLater();
    m_displayNotifier = nullptr;
  }

  if (m_currentOffer) {
    zwlr_data_control_offer_v1_destroy(m_currentOffer);
    m_currentOffer = nullptr;
  }

  if (m_device) {
    zwlr_data_control_device_v1_destroy(m_device);
    m_device = nullptr;
  }

  if (m_manager) {
    zwlr_data_control_manager_v1_destroy(m_manager);
    m_manager = nullptr;
  }

  if (m_registry) {
    wl_registry_destroy(m_registry);
    m_registry = nullptr;
  }

  if (m_seat) {
    wl_seat_destroy(m_seat);
    m_seat = nullptr;
  }

  if (m_display) {
    wl_display_disconnect(m_display);
    m_display = nullptr;
  }

  m_started = false;

  return true;
}

void WlrClipboardServer::onDataOffer(zwlr_data_control_offer_v1 *offer) {
  if (m_currentOffer && m_currentOffer != offer) { zwlr_data_control_offer_v1_destroy(m_currentOffer); }

  m_currentOffer = offer;
  m_pending.mimeTypes.clear();

  if (offer) { zwlr_data_control_offer_v1_add_listener(offer, &offer_listener, this); }
}

void WlrClipboardServer::onOfferMimeType(zwlr_data_control_offer_v1 *offer, const char *mime_type) {
  QString mimeStr = QString::fromUtf8(mime_type);
  if (isLegacyContentType(mimeStr)) { return; }
  if (mimeStr == "vicinae/concealed") {
    m_pending.mimeTypes.clear();
    return;
  }
  m_pending.mimeTypes.push_back(mimeStr);
}

void WlrClipboardServer::onSelectionChanged(zwlr_data_control_offer_v1 *offer) {
  cancelPendingReads();

  if (std::ranges::find(m_pending.mimeTypes, Clipboard::SELECTION_TOKEN_MIME_TYPE) !=
      m_pending.mimeTypes.end()) {
    qWarning() << "Found" << Clipboard::SELECTION_TOKEN_MIME_TYPE << "so ignoring onSelectionChanged";
    return;
  }

  if (!offer) {
    m_pending.mimeTypes.clear();
    return;
  }

  if (isVicinaeFocused()) { return; }

  if (offer != m_currentOffer) { return; }

  QTimer::singleShot(0, this, [this]() { receiveOffersAsync(); });
}

void WlrClipboardServer::cancelPendingReads() { m_pending.cancelReads(); }

void WlrClipboardServer::receiveOffersAsync() {
  if (m_pending.mimeTypes.empty()) { return; }

  m_pending.selection.offers.clear();
  m_pending.offerCount = 0;
  m_pending.receivedCount = 0;

  std::set<QString> filteredMimes;
  bool hasImage = false;

  for (const auto &mime : m_pending.mimeTypes) {
    if (mime.startsWith("image/")) {
      if (hasImage) { continue; }
      hasImage = true;
    }
    filteredMimes.insert(mime);
  }

  if (filteredMimes.contains("text/plain") && filteredMimes.contains("text/plain;charset=utf-8")) {
    filteredMimes.erase("text/plain");
  }

  m_pending.offerCount = filteredMimes.size();

  for (const auto &mime : filteredMimes) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
      qCritical() << "Failed to create pipe:" << strerror(errno);
      continue;
    }

    zwlr_data_control_offer_v1_receive(m_currentOffer, mime.toUtf8().constData(), pipefd[1]);
    close(pipefd[1]);

    auto notifier = QSharedPointer<QSocketNotifier>::create(pipefd[0], QSocketNotifier::Read, this);
    m_pending.activeNotifiers.push_back(notifier);

    QString mimeType = mime;
    int readFd = pipefd[0];
    auto buffer = std::make_shared<QByteArray>();

    connect(notifier.data(), &QSocketNotifier::activated, this, [this, notifier, readFd, mimeType, buffer]() {
      ssize_t n = read(readFd, m_readBuffer.data(), m_readBuffer.size());

      if (n > 0) {
        buffer->append(m_readBuffer.data(), n);
        return;
      }

      if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        qWarning() << "Read error for" << mimeType << ":" << strerror(errno);
      }

      notifier->setEnabled(false);
      close(readFd);

      auto it = std::find(m_pending.activeNotifiers.begin(), m_pending.activeNotifiers.end(), notifier);
      if (it != m_pending.activeNotifiers.end()) { m_pending.activeNotifiers.erase(it); }

      ClipboardDataOffer offer;
      offer.mimeType = mimeType;
      offer.data = *buffer;
      m_pending.selection.offers.push_back(offer);

      m_pending.receivedCount++;

      if (m_pending.receivedCount >= m_pending.offerCount) { processSelection(); }
    });
  }

  int flushResult = wl_display_flush(m_display);
  if (flushResult < 0) { qWarning() << "Failed to flush display:" << strerror(errno); }

  int err = wl_display_get_error(m_display);
  if (err) { qWarning() << "Display has error:" << err << strerror(err); }

  m_pending.timeoutTimer = new QTimer(this);
  m_pending.timeoutTimer->setInterval(10000);
  connect(m_pending.timeoutTimer, &QTimer::timeout, this, [this]() {
    qWarning() << "Clipboard receive timeout";
    cancelPendingReads();
  });
  m_pending.timeoutTimer->start();
}

void WlrClipboardServer::processSelection() {
  if (m_pending.selection.offers.empty()) { return; }

  emit selectionAdded(m_pending.selection);

  m_pending.selection.offers.clear();
  m_pending.mimeTypes.clear();
}

bool WlrClipboardServer::isLegacyContentType(const QString &str) {
  if (str.startsWith("-x") || str.startsWith("-X")) { return false; }

  return str.isUpper() && !str.contains('/');
}

bool WlrClipboardServer::isVicinaeFocused() { return QApplication::activeWindow() != nullptr; }

void WlrClipboardServer::qtClipboardChanged() {
  static QString previousToken;
  auto clip = QApplication::clipboard();

  if (!clip->ownsClipboard()) return;

  auto mimeData = clip->mimeData();

  if (!mimeData) return;

  ClipboardSelection selection;

  if (mimeData->hasFormat(Clipboard::SELECTION_TOKEN_MIME_TYPE)) {
    auto token = mimeData->data(Clipboard::SELECTION_TOKEN_MIME_TYPE);
    if (token == previousToken) return;
    previousToken = token;
  }

  for (const auto &format : mimeData->formats()) {
    if (format.startsWith("image/")) {
      ClipboardDataOffer offer;
      offer.mimeType = format;
      offer.data = mimeData->data(format);
      selection.offers.emplace_back(offer);
      break;
    }
  }

  if (mimeData->hasText()) {
    selection.offers.emplace_back(
        ClipboardDataOffer{.mimeType = "text/plain;charset=utf-8", .data = mimeData->text().toUtf8()});
  }

  if (mimeData->hasHtml()) {
    selection.offers.emplace_back(
        ClipboardDataOffer{.mimeType = "text/html", .data = mimeData->html().toUtf8()});
  }

  if (mimeData->hasUrls()) {
    ClipboardDataOffer offer;
    offer.mimeType = "text/uri-list";
    for (const auto &url : mimeData->urls()) {
      if (!offer.data.isEmpty()) { offer.data += ';'; }
      offer.data += url.toString().toUtf8();
    }
    selection.offers.emplace_back(offer);
  }

  auto isIndexableFormat = [](const QString &fmt) {
    return !isLegacyContentType(fmt) && !fmt.startsWith("text/") && !fmt.startsWith("image/");
  };

  for (const auto &format : mimeData->formats()) {
    if (isIndexableFormat(format)) {
      QByteArray data = mimeData->data(format);
      selection.offers.emplace_back(ClipboardDataOffer{format, data});
    }
  }

  if (selection.offers.empty()) { return; }

  QCryptographicHash hash(QCryptographicHash::Sha256);
  for (const auto &offer : selection.offers) {
    hash.addData(offer.mimeType.toUtf8());
    hash.addData(offer.data);
  }
  QByteArray currentHash = hash.result();

  if (currentHash == m_lastQtClipboardHash) { return; }

  m_lastQtClipboardHash = currentHash;

  qDebug() << "got qt clipboard data" << mimeData->formats();

  emit selectionAdded(selection);
}
