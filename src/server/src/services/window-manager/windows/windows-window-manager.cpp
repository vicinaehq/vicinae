#include "windows-window-manager.hpp"
#include "qml/launcher-window-platform.hpp"
#include <QTimer>
#include <algorithm>
#include <appmodel.h>
#include <dwmapi.h>
#include <propkey.h>
#include <shellapi.h>
#include <wrl/client.h>

static constexpr int REBUILD_DEBOUNCE_MS = 150;
static constexpr int WORKSPACE_SWITCH_HIDE_DELAY_MS = 300;
static constexpr const wchar_t *HELPER_CLASS_NAME = L"VicinaeWorkspaceSwitcher";
static constexpr const wchar_t *UWP_CORE_WINDOW_CLASS = L"Windows.UI.Core.CoreWindow";

namespace {

Win::WindowManager *s_instance = nullptr;

void CALLBACK winEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
  if (!s_instance || !hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
  s_instance->handleWinEvent(event, hwnd);
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

class WinWorkspace : public AbstractWindowManager::AbstractWorkspace {
public:
  WinWorkspace(QString id, QString name) : m_id(std::move(id)), m_name(std::move(name)) {}

  QString id() const override { return m_id; }
  QString name() const override { return m_name; }

private:
  QString m_id;
  QString m_name;
};

// Qt keeps each screen's native origin in logical space and scales only sizes
// (QHighDpiScaling: logical = origin + (native - origin) / factor), so a screen's
// geometry().topLeft() is valid in both coordinate spaces.
QScreen *screenForNativePoint(const QPoint &native) {
  for (QScreen *screen : QGuiApplication::screens()) {
    const QRect nativeRect(screen->geometry().topLeft(),
                           screen->geometry().size() * screen->devicePixelRatio());
    if (nativeRect.contains(native)) return screen;
  }
  return QGuiApplication::primaryScreen();
}

AbstractWindowManager::WindowBounds nativeToLogical(const RECT &native) {
  const QPoint topLeft(native.left, native.top);
  QScreen *screen = screenForNativePoint(topLeft);
  const QPointF origin = screen->geometry().topLeft();
  const qreal dpr = screen->devicePixelRatio();
  const QPointF logical = origin + (QPointF(topLeft) - origin) / dpr;

  return {.x = qRound(logical.x()),
          .y = qRound(logical.y()),
          .width = qRound((native.right - native.left) / dpr),
          .height = qRound((native.bottom - native.top) / dpr)};
}

RECT logicalToNative(const AbstractWindowManager::WindowBounds &bounds) {
  QScreen *screen =
      QGuiApplication::screenAt(QPoint(bounds.x + bounds.width / 2, bounds.y + bounds.height / 2));
  if (!screen) screen = QGuiApplication::screenAt(QPoint(bounds.x, bounds.y));
  if (!screen) screen = QGuiApplication::primaryScreen();

  const QPointF origin = screen->geometry().topLeft();
  const qreal dpr = screen->devicePixelRatio();
  const QPointF native = origin + (QPointF(bounds.x, bounds.y) - origin) * dpr;

  RECT rect;
  rect.left = qRound(native.x());
  rect.top = qRound(native.y());
  rect.right = rect.left + qRound(bounds.width * dpr);
  rect.bottom = rect.top + qRound(bounds.height * dpr);
  return rect;
}

QString windowAumid(HWND hwnd) {
  Microsoft::WRL::ComPtr<IPropertyStore> store;
  if (FAILED(SHGetPropertyStoreForWindow(hwnd, IID_PPV_ARGS(store.GetAddressOf())))) return {};

  PROPVARIANT var;
  PropVariantInit(&var);
  QString result;
  if (SUCCEEDED(store->GetValue(PKEY_AppUserModel_ID, &var)) && var.vt == VT_LPWSTR && var.pwszVal) {
    result = QString::fromWCharArray(var.pwszVal);
  }
  PropVariantClear(&var);
  return result;
}

bool sameWindows(const AbstractWindowManager::WindowList &a, const AbstractWindowManager::WindowList &b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); ++i) {
    const auto &lhs = static_cast<const Win::Window &>(*a[i]);
    const auto &rhs = static_cast<const Win::Window &>(*b[i]);
    if (lhs.hwnd() != rhs.hwnd() || lhs.title() != rhs.title() || lhs.workspace() != rhs.workspace()) {
      return false;
    }
  }
  return true;
}

} // namespace

namespace Win {

WindowManager::~WindowManager() {
  for (auto hook : m_hooks) {
    if (hook) UnhookWinEvent(hook);
  }
  if (m_helper) DestroyWindow(m_helper);
  if (s_instance == this) s_instance = nullptr;
}

void WindowManager::start() {
  s_instance = this;

  m_desktops = std::make_unique<VirtualDesktops>();
  connect(m_desktops.get(), &VirtualDesktops::changed, this, [this]() { scheduleRebuild(); });

  m_rebuildTimer = new QTimer(this);
  m_rebuildTimer->setSingleShot(true);
  m_rebuildTimer->setInterval(REBUILD_DEBOUNCE_MS);
  connect(m_rebuildTimer, &QTimer::timeout, this, [this]() { rebuildCache(); });

  auto hook = [](DWORD min, DWORD max) {
    return SetWinEventHook(min, max, nullptr, winEventProc, 0, 0,
                           WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
  };
  m_hooks = {
      hook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND),
      hook(EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND),
      hook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND),
      hook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE),
      hook(EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE),
  };

  rebuildCache();
}

void WindowManager::handleWinEvent(DWORD event, HWND hwnd) {
  if (event == EVENT_SYSTEM_FOREGROUND) {
    emit focusChanged();
    scheduleRebuild();
    return;
  }

  // title changes on child windows (taskbar clock and the like) are irrelevant
  if (event == EVENT_OBJECT_NAMECHANGE && GetAncestor(hwnd, GA_ROOT) != hwnd) return;

  scheduleRebuild();
}

void WindowManager::scheduleRebuild() const {
  // don't restart an armed timer: a sustained event stream would starve the rebuild forever
  if (m_rebuildTimer && !m_rebuildTimer->isActive()) m_rebuildTimer->start();
}

void WindowManager::rebuildCache() {
  std::vector<HWND> handles;
  EnumWindows(
      [](HWND hwnd, LPARAM lparam) -> BOOL {
        reinterpret_cast<std::vector<HWND> *>(lparam)->push_back(hwnd);
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&handles));

  WindowList windows;
  for (HWND hwnd : handles) {
    if (isListable(hwnd)) windows.emplace_back(buildWindow(hwnd));
  }

  std::erase_if(m_keyByPid, [&](const auto &entry) {
    return std::ranges::none_of(windows,
                                [&](const auto &win) { return win->pid() == static_cast<int>(entry.first); });
  });

  // always swap so bounds stay fresh; only notify on changes views care about
  const bool changed = !sameWindows(m_cache, windows);
  m_cache = std::move(windows);
  if (changed) emit windowsChanged();
}

bool WindowManager::isListable(HWND hwnd) const {
  if (!IsWindowVisible(hwnd)) return false;
  if (GetWindow(hwnd, GW_OWNER)) return false;

  const LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
  if ((exStyle & WS_EX_TOOLWINDOW) && !(exStyle & WS_EX_APPWINDOW)) return false;
  if (GetWindowTextLengthW(hwnd) == 0) return false;

  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  if (!pid || pid == GetCurrentProcessId()) return false;

  // cloaked windows are kept when they live on another virtual desktop, or when they are the
  // frame of a suspended UWP app — alt-tab still lists those and focusing them resumes the app.
  // Bare cloaked CoreWindows (dormant system apps) are dropped.
  DWORD cloaked = 0;
  DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
  if (cloaked && (!m_desktops || m_desktops->isWindowOnCurrentDesktop(hwnd))) {
    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, static_cast<int>(std::size(cls)));
    if (wcscmp(cls, L"ApplicationFrameWindow") != 0) return false;
  }

  return true;
}

std::shared_ptr<Window> WindowManager::buildWindow(HWND hwnd) const {
  wchar_t title[512] = {};
  GetWindowTextW(hwnd, title, static_cast<int>(std::size(title)));

  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);

  // minimized windows are parked at (-32000,-32000); no bounds is the honest value
  std::optional<WindowBounds> bounds;
  RECT frame{};
  if (!IsIconic(hwnd) &&
      (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &frame, sizeof(frame))) ||
       GetWindowRect(hwnd, &frame))) {
    bounds = nativeToLogical(frame);
  }

  std::optional<QString> workspace;
  if (hasWorkspaces()) workspace = m_desktops->windowDesktopId(hwnd);

  return std::make_shared<Window>(hwnd, QString::fromWCharArray(title), appKeyFor(hwnd, pid),
                                  static_cast<int>(pid), bounds, std::move(workspace));
}

QString WindowManager::keyForPid(DWORD pid) const {
  if (auto it = m_keyByPid.find(pid); it != m_keyByPid.end()) return it->second;

  QString key;
  if (HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid)) {
    UINT32 length = 0;
    if (GetApplicationUserModelId(process, &length, nullptr) == ERROR_INSUFFICIENT_BUFFER) {
      std::wstring aumid(length, L'\0');
      if (GetApplicationUserModelId(process, &length, aumid.data()) == ERROR_SUCCESS) {
        key = QString::fromWCharArray(aumid.c_str());
      }
    }
    if (key.isEmpty()) {
      wchar_t path[MAX_PATH] = {};
      DWORD size = static_cast<DWORD>(std::size(path));
      if (QueryFullProcessImageNameW(process, 0, path, &size)) {
        key = QString::fromWCharArray(path, size).toLower();
      }
    }
    CloseHandle(process);
  }

  m_keyByPid.emplace(pid, key);
  return key;
}

QString WindowManager::appKeyFor(HWND hwnd, DWORD pid) const {
  QString key = keyForPid(pid);

  // UWP app windows belong to the frame host; the real app identity is the hosted app's AUMID,
  // taken from the frame's property store (survives suspension) or the CoreWindow child's process
  if (key.endsWith(QLatin1String("\\applicationframehost.exe"))) {
    if (QString aumid = windowAumid(hwnd); !aumid.isEmpty()) return aumid;

    HWND core = nullptr;
    EnumChildWindows(hwnd, findCoreWindowProc, reinterpret_cast<LPARAM>(&core));

    DWORD corePid = 0;
    if (core) GetWindowThreadProcessId(core, &corePid);
    if (corePid && corePid != pid) {
      if (QString coreKey = keyForPid(corePid); !coreKey.isEmpty()) return coreKey;
    }
  }

  return key;
}

AbstractWindowManager::WindowPtr WindowManager::getFocusedWindowSync() const {
  HWND foreground = GetForegroundWindow();
  if (!foreground) return nullptr;

  DWORD pid = 0;
  GetWindowThreadProcessId(foreground, &pid);
  // reporting nullptr while we hold focus is what makes focus handoff detection work
  if (!pid || pid == GetCurrentProcessId()) return nullptr;

  return buildWindow(foreground);
}

void WindowManager::focusWindowSync(const AbstractWindow &window) const {
  HWND hwnd = static_cast<const Window &>(window).hwnd();

  if (IsIconic(hwnd)) ShowWindow(hwnd, SW_RESTORE);
  LauncherWindowPlatform::grantForeground();
  SetForegroundWindow(hwnd);
}

bool WindowManager::closeWindow(const AbstractWindow &window) const {
  HWND hwnd = static_cast<const Window &>(window).hwnd();
  return PostMessageW(hwnd, WM_CLOSE, 0, 0) != 0;
}

bool WindowManager::setWindowBounds(const AbstractWindow &window, const WindowBounds &bounds) const {
  HWND hwnd = static_cast<const Window &>(window).hwnd();

  if (IsZoomed(hwnd)) ShowWindow(hwnd, SW_RESTORE);

  const RECT target = logicalToNative(bounds);

  // requested bounds are visual (extended frame); SetWindowPos takes the window rect, which
  // extends past the visual frame by the invisible resize borders
  RECT windowRect{};
  if (!GetWindowRect(hwnd, &windowRect)) return false;
  RECT frame = windowRect;
  DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &frame, sizeof(frame));

  const int left = frame.left - windowRect.left;
  const int top = frame.top - windowRect.top;
  const int right = windowRect.right - frame.right;
  const int bottom = windowRect.bottom - frame.bottom;

  return SetWindowPos(hwnd, nullptr, target.left - left, target.top - top,
                      (target.right - target.left) + left + right,
                      (target.bottom - target.top) + top + bottom, SWP_NOZORDER | SWP_NOACTIVATE) != 0;
}

AbstractWindowManager::WorkspaceList WindowManager::listWorkspaces() const {
  WorkspaceList workspaces;
  if (!hasWorkspaces()) return workspaces;

  for (const auto &desktop : m_desktops->desktops()) {
    workspaces.emplace_back(std::make_shared<WinWorkspace>(desktop.id, desktop.name));
  }
  return workspaces;
}

AbstractWindowManager::WorkspacePtr WindowManager::getActiveWorkspace() const {
  if (!hasWorkspaces()) return nullptr;

  const QString active = m_desktops->activeDesktopId();
  for (const auto &desktop : m_desktops->desktops()) {
    if (desktop.id.compare(active, Qt::CaseInsensitive) == 0) {
      return std::make_shared<WinWorkspace>(desktop.id, desktop.name);
    }
  }
  return nullptr;
}

HWND WindowManager::ensureHelperWindow() const {
  if (m_helper) return m_helper;

  WNDCLASSW wc{};
  wc.lpfnWndProc = DefWindowProcW;
  wc.hInstance = GetModuleHandleW(nullptr);
  wc.lpszClassName = HELPER_CLASS_NAME;
  RegisterClassW(&wc);

  m_helper = CreateWindowExW(WS_EX_TOOLWINDOW, HELPER_CLASS_NAME, L"", WS_POPUP, -32000, -32000, 1, 1,
                             nullptr, nullptr, wc.hInstance, nullptr);
  return m_helper;
}

bool WindowManager::activateWorkspace(const QString &workspaceId) const {
  if (!hasWorkspaces()) return false;

  // there is no stable API to switch desktops directly, but focusing a window that lives on the
  // target desktop makes the shell switch to it — so we briefly plant our own helper window there
  HWND helper = ensureHelperWindow();
  if (!helper) return false;
  if (!m_desktops->moveOwnWindowToDesktop(helper, workspaceId)) return false;

  ShowWindow(helper, SW_SHOW);
  LauncherWindowPlatform::grantForeground();
  SetForegroundWindow(helper);
  QTimer::singleShot(WORKSPACE_SWITCH_HIDE_DELAY_MS, [helper]() { ShowWindow(helper, SW_HIDE); });

  return true;
}

} // namespace Win
