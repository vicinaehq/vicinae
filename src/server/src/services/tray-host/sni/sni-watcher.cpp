#include "sni-watcher.hpp"
#include <QDBusConnection>
#include <QDBusMessage>

static constexpr const char *WATCHER_SERVICE = "org.kde.StatusNotifierWatcher";
static constexpr const char *WATCHER_PATH = "/StatusNotifierWatcher";

SniWatcher::SniWatcher(QObject *parent) : QObject(parent) {
  m_serviceWatcher.setConnection(QDBusConnection::sessionBus());
  m_serviceWatcher.setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
  connect(&m_serviceWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &SniWatcher::serviceGone);
}

SniWatcher::~SniWatcher() { release(); }

bool SniWatcher::tryClaim() {
  if (m_owned) return true;

  auto bus = QDBusConnection::sessionBus();
  if (!bus.registerObject(WATCHER_PATH, this,
                          QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllProperties |
                              QDBusConnection::ExportAllSignals)) {
    return false;
  }
  if (!bus.registerService(WATCHER_SERVICE)) {
    bus.unregisterObject(WATCHER_PATH);
    return false;
  }

  m_owned = true;
  return true;
}

void SniWatcher::release() {
  if (!m_owned) return;
  auto bus = QDBusConnection::sessionBus();
  bus.unregisterService(WATCHER_SERVICE);
  bus.unregisterObject(WATCHER_PATH);
  m_owned = false;
  m_items.clear();
  m_hosts.clear();
  for (const auto &name : m_serviceWatcher.watchedServices()) {
    m_serviceWatcher.removeWatchedService(name);
  }
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
