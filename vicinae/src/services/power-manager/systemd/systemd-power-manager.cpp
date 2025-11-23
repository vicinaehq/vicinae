#include "systemd-power-manager.hpp"
#include <cstdint>
#include <fstream>
#include <qdbusargument.h>
#include <qdbusmetatype.h>
#include <qtdbusglobal.h>
#include <unistd.h>

static const constexpr uint64_t SD_LOGIND_SOFT_REBOOT = 1 << 2;

SystemdPowerManager::SystemdPowerManager() : m_sessionId(getUserSessionId()) {
  m_iface =
      std::make_unique<QDBusInterface>(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE, QDBusConnection::systemBus());
  if (!m_iface->isValid()) {
    qWarning() << "Failed to create D-Bus interface:" << m_iface->lastError().message();
  }
}

bool SystemdPowerManager::can(const QString &method) const {
  auto reply = m_iface->call(method);
  auto args = reply.arguments();

  if (args.isEmpty()) return false;

  return true;
}

bool SystemdPowerManager::powerOff() { return m_iface->call("PowerOff", false).errorMessage().isEmpty(); }
bool SystemdPowerManager::reboot() { return m_iface->call("Reboot", false).errorMessage().isEmpty(); }
bool SystemdPowerManager::sleep() const { return m_iface->call("Sleep", false).errorMessage().isEmpty(); }
bool SystemdPowerManager::suspend() { return m_iface->call("Suspend", false).errorMessage().isEmpty(); }
bool SystemdPowerManager::hibernate() { return m_iface->call("Hibernate", false).errorMessage().isEmpty(); }
bool SystemdPowerManager::lock() {
  if (!m_sessionId) { return false; }
  return m_iface->call("LockSession", *m_sessionId).errorMessage().isEmpty();
}

bool SystemdPowerManager::logout() {
  if (!m_sessionId) { return false; }
  return m_iface->call("TerminateSession", *m_sessionId).errorMessage().isEmpty();
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
bool SystemdPowerManager::canLock() const { return m_sessionId.has_value(); }
bool SystemdPowerManager::canLogOut() const { return m_sessionId.has_value(); }

QString SystemdPowerManager::id() const { return "systemd"; }

std::optional<uint32_t> SystemdPowerManager::currentSessionId() const { return m_sessionId; }

std::optional<uint32_t> SystemdPowerManager::getUserSessionId() const {
  std::ifstream ifs("/proc/self/sessionid");
  if (ifs) {
    uint32_t n = 0;
    ifs >> n;
    return n;
  }
  return {};
}
