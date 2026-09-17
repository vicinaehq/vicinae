#include "windows-power-manager.hpp"
#include <windows.h>
#include <powrprof.h>

namespace {

constexpr DWORD SHUTDOWN_REASON =
    SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER | SHTDN_REASON_FLAG_PLANNED;

bool enableShutdownPrivilege() {
  HANDLE token = nullptr;
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) return false;

  TOKEN_PRIVILEGES priv{};
  priv.PrivilegeCount = 1;
  priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  bool ok = LookupPrivilegeValueW(nullptr, SE_SHUTDOWN_NAME, &priv.Privileges[0].Luid) &&
            AdjustTokenPrivileges(token, FALSE, &priv, 0, nullptr, nullptr) &&
            GetLastError() == ERROR_SUCCESS;
  CloseHandle(token);
  return ok;
}

bool exitWindows(UINT flags) {
  if (!enableShutdownPrivilege()) return false;
  return ExitWindowsEx(flags | EWX_FORCEIFHUNG, SHUTDOWN_REASON) != 0;
}

SYSTEM_POWER_CAPABILITIES powerCapabilities() {
  SYSTEM_POWER_CAPABILITIES caps{};
  GetPwrCapabilities(&caps);
  return caps;
}

} // namespace

bool WindowsPowerManager::powerOff() { return exitWindows(EWX_POWEROFF); }
bool WindowsPowerManager::reboot() { return exitWindows(EWX_REBOOT); }
bool WindowsPowerManager::logout() {
  return ExitWindowsEx(EWX_LOGOFF | EWX_FORCEIFHUNG, SHUTDOWN_REASON) != 0;
}
bool WindowsPowerManager::sleep() const { return SetSuspendState(FALSE, FALSE, FALSE) != 0; }
bool WindowsPowerManager::suspend() { return sleep(); }
bool WindowsPowerManager::hibernate() { return SetSuspendState(TRUE, FALSE, FALSE) != 0; }
bool WindowsPowerManager::lock() { return LockWorkStation() != 0; }

bool WindowsPowerManager::canPowerOff() const { return true; }
bool WindowsPowerManager::canReboot() const { return true; }
bool WindowsPowerManager::canLock() const { return true; }
bool WindowsPowerManager::canLogOut() const { return true; }
bool WindowsPowerManager::canSleep() const {
  auto caps = powerCapabilities();
  return caps.SystemS1 || caps.SystemS2 || caps.SystemS3 || caps.AoAc;
}
bool WindowsPowerManager::canSuspend() const { return canSleep(); }
bool WindowsPowerManager::canHibernate() const {
  auto caps = powerCapabilities();
  return caps.SystemS4 && caps.HiberFilePresent;
}

QString WindowsPowerManager::id() const { return "windows"; }
