#include "systemd-power-manager.hpp"
#include <cstdint>
#include <fstream>
#include <qdbusargument.h>
#include <qdbusmetatype.h>
#include <qtdbusglobal.h>
#include <unistd.h>

static const constexpr uint64_t SD_LOGIND_SOFT_REBOOT = 1 << 2;

SystemdPowerManager::SystemdPowerManager() : m_sessionId(getUserSessionId()) {}

bool SystemdPowerManager::can(const QString &method) const {
  auto reply = logindCall(method);
  auto args = reply.arguments();

  if (args.isEmpty()) return false;

  return true;
}

bool SystemdPowerManager::powerOff() { return logindCall("PowerOff", false).errorMessage().isEmpty(); }
bool SystemdPowerManager::reboot() { return logindCall("Reboot", false).errorMessage().isEmpty(); }
bool SystemdPowerManager::sleep() const { return logindCall("Sleep", false).errorMessage().isEmpty(); }
bool SystemdPowerManager::suspend() { return logindCall("Suspend", false).errorMessage().isEmpty(); }
bool SystemdPowerManager::hibernate() { return logindCall("Hibernate", false).errorMessage().isEmpty(); }
bool SystemdPowerManager::lock() {
  if (!m_sessionId) { return false; }
  return logindCall("LockSession", *m_sessionId).errorMessage().isEmpty();
}

bool SystemdPowerManager::logout() {
  if (!m_sessionId) { return false; }
  return logindCall("TerminateSession", *m_sessionId).errorMessage().isEmpty();
}

bool SystemdPowerManager::softReboot() {
  return logindCall("RebootWithFlags", static_cast<quint64>(SD_LOGIND_SOFT_REBOOT)).errorMessage().isEmpty();
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
