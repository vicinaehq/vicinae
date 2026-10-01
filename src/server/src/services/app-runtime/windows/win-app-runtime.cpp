#include "win-app-runtime.hpp"
#include "services/app-service/app-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "services/window-manager/windows/windows-window.hpp"
#include <optional>
#include <unordered_set>
#include <windows.h>

static constexpr const wchar_t *UWP_CORE_WINDOW_CLASS = L"Windows.UI.Core.CoreWindow";

namespace {

bool isFrameHostPid(DWORD pid) {
  HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
  if (!process) return false;

  wchar_t path[MAX_PATH] = {};
  DWORD size = static_cast<DWORD>(std::size(path));
  bool result = QueryFullProcessImageNameW(process, 0, path, &size) &&
                QString::fromWCharArray(path, size)
                    .endsWith(QLatin1String("\\applicationframehost.exe"), Qt::CaseInsensitive);
  CloseHandle(process);
  return result;
}

BOOL CALLBACK findCoreWindowProc(HWND child, LPARAM lparam) {
  wchar_t cls[64] = {};
  if (GetClassNameW(child, cls, static_cast<int>(std::size(cls))) &&
      wcscmp(cls, UWP_CORE_WINDOW_CLASS) == 0) {
    *reinterpret_cast<HWND *>(lparam) = child;
    return FALSE;
  }
  return TRUE;
}

// A UWP frame's pid is ApplicationFrameHost's; terminating it would take down every UWP window.
// The hosted app's process owns the CoreWindow child.
std::optional<DWORD> terminatablePid(HWND hwnd, DWORD pid) {
  if (!isFrameHostPid(pid)) return pid;

  HWND core = nullptr;
  EnumChildWindows(hwnd, findCoreWindowProc, reinterpret_cast<LPARAM>(&core));

  DWORD corePid = 0;
  if (core) GetWindowThreadProcessId(core, &corePid);
  if (corePid && corePid != pid) return corePid;
  return std::nullopt;
}

} // namespace

WindowsAppRuntime::WindowsAppRuntime(WindowManager &wm, AppService &appService)
    : m_wm(wm), m_appService(appService) {
  connect(&m_wm, &WindowManager::windowsChanged, this, &WindowsAppRuntime::runningAppsChanged);
  connect(&m_wm, &WindowManager::focusChanged, this, &WindowsAppRuntime::frontmostAppChanged);
}

bool WindowsAppRuntime::isRunning(const AbstractApplication &app) const {
  return !m_wm.findAppWindows(app).empty();
}

std::shared_ptr<AbstractApplication> WindowsAppRuntime::frontmostApp() const {
  auto focused = m_wm.getFocusedWindow();
  if (!focused) return nullptr;
  return m_appService.findByClass(focused->wmClass());
}

bool WindowsAppRuntime::activate(const AbstractApplication &app) const {
  auto wins = m_wm.findAppWindows(app);
  if (wins.empty()) return false;
  m_wm.provider()->focusWindowSync(*wins.front());
  return true;
}

bool WindowsAppRuntime::quit(const AbstractApplication &app) const {
  bool closed = false;
  for (const auto &win : m_wm.findAppWindows(app)) {
    if (m_wm.provider()->closeWindow(*win)) closed = true;
  }
  return closed;
}

bool WindowsAppRuntime::forceQuit(const AbstractApplication &app) const {
  std::unordered_set<DWORD> pids;
  bool acted = false;

  for (const auto &win : m_wm.findAppWindows(app)) {
    const auto &window = static_cast<const Win::Window &>(*win);

    std::optional<DWORD> target;
    if (auto pid = window.pid()) target = terminatablePid(window.hwnd(), static_cast<DWORD>(*pid));

    if (target) {
      pids.emplace(*target);
    } else if (m_wm.provider()->closeWindow(window)) {
      acted = true;
    }
  }

  for (DWORD pid : pids) {
    if (HANDLE process = OpenProcess(PROCESS_TERMINATE, FALSE, pid)) {
      if (TerminateProcess(process, 1)) acted = true;
      CloseHandle(process);
    }
  }
  return acted;
}
