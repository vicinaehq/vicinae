#include "systemd-power-manager.hpp"
#include <cstdint>
#include <qdbusargument.h>
#include <qdbusmetatype.h>
#include <qtdbusglobal.h>
#include <unistd.h>

static const constexpr uint64_t SD_LOGIND_SOFT_REBOOT = 1 << 2;

bool SystemdPowerManager::powerOff() { return m_iface->call("PowerOff", false).errorMessage().isEmpty(); }
bool SystemdPowerManager::reboot() { return m_iface->call("Reboot", false).errorMessage().isEmpty(); }
bool SystemdPowerManager::sleep() const { return m_iface->call("Sleep", static_cast<quint64>(0)).errorMessage().isEmpty(); }
bool SystemdPowerManager::suspend() { return m_iface->call("Suspend", false).errorMessage().isEmpty(); }
bool SystemdPowerManager::hibernate() { return m_iface->call("Hibernate", false).errorMessage().isEmpty(); }
bool SystemdPowerManager::lock() {
  auto session = getUserSession();

  if (!session) { return false; }

  return m_iface->call("LockSession", session->id).errorMessage().isEmpty();
}

bool SystemdPowerManager::logout() {
  return m_iface->call("TerminateUser", getuid()).errorMessage().isEmpty();
}

bool SystemdPowerManager::softReboot() {
  return m_iface->call("RebootWithFlags", static_cast<quint64>(SD_LOGIND_SOFT_REBOOT))
      .errorMessage()
      .isEmpty();
}

bool SystemdPowerManager::canPowerOff() const { return can("CanPowerOff"); }
bool SystemdPowerManager::canSuspend() const { return can("CanSuspend"); }
bool SystemdPowerManager::canSleep() const { return can("CanSleep"); }
bool SystemdPowerManager::canHibernate() const { return can("CanHibernate"); }
bool SystemdPowerManager::canReboot() const { return can("CanReboot"); }
bool SystemdPowerManager::canSoftReboot() const { return canReboot(); }
bool SystemdPowerManager::canLock() const { return true; }
bool SystemdPowerManager::canLogOut() const { return true; }

QString SystemdPowerManager::id() const { return "systemd"; }

SystemdPowerManager::SystemdPowerManager() {
  m_iface =
      std::make_unique<QDBusInterface>(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE, QDBusConnection::systemBus());

  if (!m_iface->isValid()) {
    qWarning() << "Failed to create D-Bus interface:" << m_iface->lastError().message();
  }
}

std::optional<SystemdPowerManager::Session> SystemdPowerManager::getUserSession() {
  auto reply = m_iface->call("ListSessions");
  auto args = reply.arguments();

  if (args.isEmpty()) return {};

  SessionList sessions;
  auto arg = args.front().value<QDBusArgument>();

  arg >> sessions;

  for (const auto &session : sessions) {
    if (session.uid == getuid() && !session.seatId.isEmpty()) { return session; }
  }

  return {};
}

bool SystemdPowerManager::can(const QString &method) const {
  auto reply = m_iface->call(method);
  auto args = reply.arguments();

  if (args.isEmpty()) return false;

  return true;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, SystemdPowerManager::SessionList &sessions) {
  sessions.clear();
  arg.beginArray();

  while (!arg.atEnd()) {
    SystemdPowerManager::Session session;
    arg >> session;
    sessions.emplace_back(session);
  }

  arg.endArray();

  return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, SystemdPowerManager::Session &session) {
  arg.beginStructure();
  arg >> session.id >> session.uid >> session.user >> session.seatId >> session.path;
  arg.endStructure();

  return arg;
}
