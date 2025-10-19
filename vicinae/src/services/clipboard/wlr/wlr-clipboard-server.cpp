#include "wlr-clipboard-server.hpp"
#include "utils/environment.hpp"
#include "wlr-data-control-unstable-v1-client-protocol.h"
#include <QApplication>
#include <QSocketNotifier>
#include <QGuiApplication>
#include <qlogging.h>
#include <algorithm>
#include <array>
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

static void data_control_device_finished(void *data, zwlr_data_control_device_v1 *device) {
  qWarning() << "wlr-data-control device finished";
}

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
    wl_display_disconnect(m_display);
    m_display = nullptr;
    return false;
  }

  m_registry = wl_display_get_registry(m_display);
  wl_registry_add_listener(m_registry, &registry_listener, this);
  wl_display_roundtrip(m_display);

  if (!m_manager) {
    qCritical()
        << "zwlr_data_control_manager_v1 is not available in your current environment vicinae think is "
        << Environment::getEnvironmentDescription()
        << "This might indicate that your environment does not implement the wlr-data-control protocol or "
           "that it is default-disabled as a security mechanism (Cosmic does this)";
    return false;
  }

  m_device = zwlr_data_control_manager_v1_get_data_device(m_manager, m_seat);
  zwlr_data_control_device_v1_add_listener(m_device, &device_listener, this);

  wl_display_roundtrip(m_display);

  int displayFd = wl_display_get_fd(m_display);
  m_displayNotifier = new QSocketNotifier(displayFd, QSocketNotifier::Read, this);
  connect(m_displayNotifier, &QSocketNotifier::activated, this, [this]() {
    qDebug() << "[WLR] Display fd readable, dispatching";
    if (wl_display_prepare_read(m_display) == 0) {
      wl_display_read_events(m_display);
      wl_display_dispatch_pending(m_display);
    } else {
      wl_display_dispatch_pending(m_display);
    }
  });

  m_started = true;
  qInfo() << "WlrClipboardServer started successfully";

  return true;
}

bool WlrClipboardServer::stop() {
  qInfo() << "Stopping WlrClipboardServer";

  cancelPendingReads();

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
  m_pending.mimeTypes.push_back(mimeStr);
}

void WlrClipboardServer::onSelectionChanged(zwlr_data_control_offer_v1 *offer) {
  qDebug() << "[WLR] Selection changed, offer:" << offer;
  cancelPendingReads();

  if (!offer) {
    qDebug() << "[WLR] Clipboard cleared";
    m_pending.mimeTypes.clear();
    return;
  }

  if (offer != m_currentOffer) {
    qWarning() << "[WLR] Selection offer mismatch";
    return;
  }

  qDebug() << "[WLR] MIME types collected:" << m_pending.mimeTypes.size();
  for (const auto &mime : m_pending.mimeTypes) {
    qDebug() << "[WLR]   -" << mime;
  }

  qDebug() << "[WLR] Deferring receive to next event loop";
  QTimer::singleShot(0, this, [this]() { receiveOffersAsync(); });
}

void WlrClipboardServer::cancelPendingReads() { m_pending.cancelReads(); }

void WlrClipboardServer::receiveOffersAsync() {
  if (m_pending.mimeTypes.empty()) {
    qDebug() << "[WLR] No MIME types to receive";
    return;
  }

  qDebug() << "[WLR] Starting async receive";
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
  qDebug() << "[WLR] Will receive" << m_pending.offerCount << "offers after filtering";

  for (const auto &mime : filteredMimes) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
      qCritical() << "[WLR] Failed to create pipe:" << strerror(errno);
      continue;
    }

    qDebug() << "[WLR] Requesting offer for" << mime << "on pipe fd" << pipefd[0];

    zwlr_data_control_offer_v1_receive(m_currentOffer, mime.toUtf8().constData(), pipefd[1]);
    close(pipefd[1]);

    auto notifier = QSharedPointer<QSocketNotifier>::create(pipefd[0], QSocketNotifier::Read, this);
    m_pending.activeNotifiers.push_back(notifier);

    QString mimeType = mime;
    int readFd = pipefd[0];
    auto buffer = std::make_shared<QByteArray>();

    qDebug() << "[WLR] Setup notifier for" << mimeType << "fd" << readFd;

    connect(notifier.data(), &QSocketNotifier::activated, this, [this, notifier, readFd, mimeType, buffer]() {
      ssize_t n = read(readFd, m_readBuffer.data(), m_readBuffer.size());

      if (n > 0) {
        qDebug() << "[WLR] Read" << n << "bytes for" << mimeType << "(total:" << (buffer->size() + n) << ")";
        buffer->append(m_readBuffer.data(), n);
        return;
      }

      if (n < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
          qWarning() << "[WLR] Read error for" << mimeType << ":" << strerror(errno);
        } else {
          qDebug() << "[WLR] EAGAIN/EWOULDBLOCK for" << mimeType;
        }
      } else {
        qDebug() << "[WLR] EOF for" << mimeType << ", total size:" << buffer->size();
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
      qDebug() << "[WLR] Received" << m_pending.receivedCount << "/" << m_pending.offerCount << "offers";

      if (m_pending.receivedCount >= m_pending.offerCount) { processSelection(); }
    });
  }

  qDebug() << "[WLR] Flushing display after setting up all notifiers";
  int flushResult = wl_display_flush(m_display);
  if (flushResult < 0) {
    qWarning() << "[WLR] Failed to flush display:" << strerror(errno);
  } else {
    qDebug() << "[WLR] Display flushed, wrote" << flushResult << "bytes";
  }

  int err = wl_display_get_error(m_display);
  if (err) { qWarning() << "[WLR] Display has error:" << err << strerror(err); }

  m_pending.timeoutTimer = new QTimer(this);
  m_pending.timeoutTimer->setInterval(100);
  connect(m_pending.timeoutTimer, &QTimer::timeout, this, [this]() {
    static int ticks = 0;
    ticks++;
    if (ticks % 10 == 0) {
      qDebug() << "[WLR] Still waiting... received" << m_pending.receivedCount << "/" << m_pending.offerCount
               << "after" << ticks / 10 << "seconds";
    }
    if (ticks >= 100) {
      qWarning() << "[WLR] Timeout! Received" << m_pending.receivedCount << "/" << m_pending.offerCount;
      qWarning() << "[WLR] Active notifiers:" << m_pending.activeNotifiers.size();
      ticks = 0;
      cancelPendingReads();
    }
  });
  m_pending.timeoutTimer->start();
  qDebug() << "[WLR] Started timeout timer with 100ms ticks";
}

void WlrClipboardServer::processSelection() {
  qDebug() << "[WLR] Processing selection with" << m_pending.selection.offers.size() << "offers";
  if (m_pending.selection.offers.empty()) {
    qWarning() << "[WLR] No offers to process!";
    return;
  }

  for (const auto &offer : m_pending.selection.offers) {
    qDebug() << "[WLR]   - Offer:" << offer.mimeType << "size:" << offer.data.size();
  }

  emit selectionAdded(m_pending.selection);

  m_pending.selection.offers.clear();
  m_pending.mimeTypes.clear();
  qDebug() << "[WLR] Selection processed and emitted";
}

bool WlrClipboardServer::isLegacyContentType(const QString &str) {
  if (str.startsWith("-x") || str.startsWith("-X")) { return false; }

  return str.isUpper() && !str.contains('/');
}
