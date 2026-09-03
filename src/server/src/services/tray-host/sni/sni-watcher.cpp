#include "sni-watcher.hpp"
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDebug>

static constexpr const char *WATCHER_SERVICE = "org.kde.StatusNotifierWatcher";
static constexpr const char *WATCHER_PATH = "/StatusNotifierWatcher";

SniWatcher::SniWatcher(QObject *parent)
    : QObject(parent), m_nameWatcher(WATCHER_SERVICE, QDBusConnection::sessionBus()) {
  connect(&m_nameWatcher, &QDBusServiceWatcher::serviceOwnerChanged, this, &SniWatcher::onNameOwnerChanged);

  m_serviceWatcher.setConnection(QDBusConnection::sessionBus());
  m_serviceWatcher.setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
  connect(&m_serviceWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &SniWatcher::serviceGone);

  m_claimTimer.setSingleShot(true);
  m_claimTimer.setInterval(CLAIM_DELAY_MS);
  connect(&m_claimTimer, &QTimer::timeout, this, &SniWatcher::tryClaim);

  m_queuePoll.setInterval(QUEUE_POLL_MS);
  connect(&m_queuePoll, &QTimer::timeout, this, &SniWatcher::pollQueuedOwners);

  auto *iface = QDBusConnection::sessionBus().interface();
  if (!iface || !iface->isServiceRegistered(WATCHER_SERVICE)) m_claimTimer.start();
}

SniWatcher::~SniWatcher() { release(); }

void SniWatcher::onNameOwnerChanged(const QString &, const QString &, const QString &newOwner) {
  const bool ours = newOwner == QDBusConnection::sessionBus().baseService();

  if (newOwner.isEmpty()) {
    if (m_owned) {
      qInfo() << "Lost StatusNotifierWatcher name";
      dropped();
    }
    m_claimTimer.start();
    return;
  }

  m_claimTimer.stop();
  if (m_owned && !ours) {
    qInfo() << "StatusNotifierWatcher taken over by" << newOwner;
    dropped();
  }
}

void SniWatcher::tryClaim() {
  if (m_owned) return;

  auto bus = QDBusConnection::sessionBus();
  auto *iface = bus.interface();
  if (!iface || iface->isServiceRegistered(WATCHER_SERVICE)) return;

  if (!bus.registerObject(WATCHER_PATH, this,
                          QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllProperties |
                              QDBusConnection::ExportAllSignals)) {
    return;
  }

  auto reply = iface->registerService(WATCHER_SERVICE, QDBusConnectionInterface::DontQueueService,
                                      QDBusConnectionInterface::AllowReplacement);
  if (!reply.isValid() || reply.value() != QDBusConnectionInterface::ServiceRegistered) {
    bus.unregisterObject(WATCHER_PATH);
    return;
  }

  qInfo() << "No StatusNotifierWatcher on the session bus, acting as tray host";
  m_owned = true;
  m_queuePoll.start();
}

void SniWatcher::release() {
  if (!m_owned) return;
  QDBusConnection::sessionBus().unregisterService(WATCHER_SERVICE);
  dropped();
}

void SniWatcher::dropped() {
  m_owned = false;
  m_queuePoll.stop();
  QDBusConnection::sessionBus().unregisterObject(WATCHER_PATH);
  m_items.clear();
  m_hosts.clear();
  for (const auto &name : m_serviceWatcher.watchedServices()) {
    m_serviceWatcher.removeWatchedService(name);
  }
}

void SniWatcher::pollQueuedOwners() {
  auto bus = QDBusConnection::sessionBus();
  auto msg = QDBusMessage::createMethodCall("org.freedesktop.DBus", "/org/freedesktop/DBus",
                                            "org.freedesktop.DBus", "ListQueuedOwners");
  msg << QString(WATCHER_SERVICE);

  auto *watcher = new QDBusPendingCallWatcher(bus.asyncCall(msg), this);
  connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *w) {
    w->deleteLater();
    QDBusPendingReply<QStringList> reply = *w;
    if (reply.isError() || !m_owned) return;

    const auto self = QDBusConnection::sessionBus().baseService();
    for (const auto &owner : reply.value()) {
      if (owner != self) {
        qInfo() << "Releasing StatusNotifierWatcher to queued owner" << owner;
        release();
        return;
      }
    }
  });
}

void SniWatcher::RegisterStatusNotifierItem(const QString &service) {
  QString busName;
  QString path;

  if (service.startsWith('/')) {
    busName = message().service();
    path = service;
  } else {
    busName = service;
    path = "/StatusNotifierItem";
  }

  const auto ref = busName + path;
  if (m_items.contains(ref)) return;

  m_items << ref;
  m_serviceWatcher.addWatchedService(busName);
  emit StatusNotifierItemRegistered(ref);
}

void SniWatcher::RegisterStatusNotifierHost(const QString &service) {
  if (m_hosts.contains(service)) return;
  m_hosts << service;
  m_serviceWatcher.addWatchedService(service);
  emit StatusNotifierHostRegistered();
}

void SniWatcher::serviceGone(const QString &name) {
  m_serviceWatcher.removeWatchedService(name);

  for (auto it = m_items.begin(); it != m_items.end();) {
    if (it->startsWith(name + "/")) {
      const auto ref = *it;
      it = m_items.erase(it);
      emit StatusNotifierItemUnregistered(ref);
    } else {
      ++it;
    }
  }

  if (m_hosts.removeAll(name) > 0 && m_hosts.isEmpty()) emit StatusNotifierHostUnregistered();
}
