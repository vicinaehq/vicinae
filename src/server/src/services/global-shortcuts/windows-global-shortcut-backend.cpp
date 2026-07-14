#include <QtLogging>
#include <future>
#include <windows.h>
#include "services/global-shortcuts/windows-global-shortcut-backend.hpp"

namespace {

std::optional<UINT> vkForQtKey(Qt::Key key) {
  if (key >= Qt::Key_A && key <= Qt::Key_Z) { return 'A' + (key - Qt::Key_A); }
  if (key >= Qt::Key_0 && key <= Qt::Key_9) { return '0' + (key - Qt::Key_0); }
  if (key >= Qt::Key_F1 && key <= Qt::Key_F12) { return VK_F1 + (key - Qt::Key_F1); }

  switch (key) {
  case Qt::Key_Space:
    return VK_SPACE;
  case Qt::Key_Return:
  case Qt::Key_Enter:
    return VK_RETURN;
  case Qt::Key_Escape:
    return VK_ESCAPE;
  case Qt::Key_Tab:
    return VK_TAB;
  case Qt::Key_Backspace:
    return VK_BACK;
  case Qt::Key_Delete:
    return VK_DELETE;
  case Qt::Key_Home:
    return VK_HOME;
  case Qt::Key_End:
    return VK_END;
  case Qt::Key_PageUp:
    return VK_PRIOR;
  case Qt::Key_PageDown:
    return VK_NEXT;
  case Qt::Key_Left:
    return VK_LEFT;
  case Qt::Key_Right:
    return VK_RIGHT;
  case Qt::Key_Up:
    return VK_UP;
  case Qt::Key_Down:
    return VK_DOWN;
  case Qt::Key_Minus:
    return VK_OEM_MINUS;
  case Qt::Key_Plus:
  case Qt::Key_Equal:
    return VK_OEM_PLUS;
  case Qt::Key_BracketLeft:
    return VK_OEM_4;
  case Qt::Key_BracketRight:
    return VK_OEM_6;
  case Qt::Key_Backslash:
    return VK_OEM_5;
  case Qt::Key_Semicolon:
    return VK_OEM_1;
  case Qt::Key_Apostrophe:
    return VK_OEM_7;
  case Qt::Key_Comma:
    return VK_OEM_COMMA;
  case Qt::Key_Period:
    return VK_OEM_PERIOD;
  case Qt::Key_Slash:
    return VK_OEM_2;
  case Qt::Key_QuoteLeft:
    return VK_OEM_3;
  default:
    return std::nullopt;
  }
}

UINT winModifiers(Qt::KeyboardModifiers mods) {
  UINT win = 0;
  if (mods.testFlag(Qt::ControlModifier)) { win |= MOD_CONTROL; }
  if (mods.testFlag(Qt::AltModifier)) { win |= MOD_ALT; }
  if (mods.testFlag(Qt::ShiftModifier)) { win |= MOD_SHIFT; }
  if (mods.testFlag(Qt::MetaModifier)) { win |= MOD_WIN; }
  return win;
}

WindowsGlobalShortcutBackend *g_backend = nullptr;

UINT currentModifiers() {
  UINT mods = 0;
  if (GetAsyncKeyState(VK_CONTROL) < 0) mods |= MOD_CONTROL;
  if (GetAsyncKeyState(VK_MENU) < 0) mods |= MOD_ALT;
  if (GetAsyncKeyState(VK_SHIFT) < 0) mods |= MOD_SHIFT;
  if (GetAsyncKeyState(VK_LWIN) < 0 || GetAsyncKeyState(VK_RWIN) < 0) mods |= MOD_WIN;
  return mods;
}

// We use a low level keyboard hook because the registered hotkey doesn't fire correctly
// under some circumstances.
// Typically, switching virtual workspaces and then immediately pressing the registered hotkey
// won't work until another shell action is performed (opening a new window, pressing Win+Tab again...)
// The hook works in all cases so we rely on that instead
LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HC_ACTION && g_backend) {
    const auto *k = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);
    const bool down = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    if (g_backend->dispatchKey(k->vkCode, currentModifiers(), down)) { return 1; }
  }
  return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

} // namespace

WindowsGlobalShortcutBackend::~WindowsGlobalShortcutBackend() {
  unbindAll();
  g_backend = nullptr;
  if (m_hookThread.joinable()) {
    PostThreadMessageW(m_hookThreadId, WM_QUIT, 0, 0);
    m_hookThread.join();
  }
}

bool WindowsGlobalShortcutBackend::start() {
  if (m_started) { return true; }

  g_backend = this;

  std::promise<bool> installed;
  auto installedFuture = installed.get_future();

  m_hookThread = std::thread([this, &installed]() {
    m_hookThreadId = GetCurrentThreadId();
    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, keyboardHookProc, GetModuleHandleW(nullptr), 0);
    installed.set_value(hook != nullptr);
    if (!hook) { return; }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {}
    UnhookWindowsHookEx(hook);
  });

  if (!installedFuture.get()) {
    qWarning() << "failed to install keyboard hook";
    m_hookThread.join();
    return false;
  }

  m_started = true;
  emit ready();
  return true;
}

bool WindowsGlobalShortcutBackend::dispatchKey(unsigned int vk, unsigned int mods, bool down) {
  std::scoped_lock lock(m_targetsMutex);

  for (auto &target : m_targets) {
    if (target.vk != vk) continue;
    if (!down) {
      target.down = false;
      continue;
    }
    if (mods == target.mods) {
      if (!target.down) {
        target.down = true;
        QMetaObject::invokeMethod(
            this, [this, id = target.id]() { emit shortcutActivated(id, GetTickCount64()); },
            Qt::QueuedConnection);
      }
      return true;
    }
  }
  return false;
}

std::expected<void, QString>
WindowsGlobalShortcutBackend::bindShortcut(const GlobalShortcutRequest &request) {
  unbindShortcut(request.id);

  const auto vk = vkForQtKey(request.trigger.key());

  if (!vk) { return std::unexpected(QStringLiteral("unsupported or invalid trigger")); }

  const UINT mods = winModifiers(request.trigger.mods());
  const int regId = m_nextRegistrationId++;

  // The source of truth to activate shortcuts is the keyboard hook but we still want
  // to register the global shortcut so that no other application can steal it.
  if (!RegisterHotKey(nullptr, regId, mods, *vk)) {
    if (GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED) {
      return std::unexpected(QStringLiteral("already registered by another application"));
    }
  }

  std::scoped_lock lock(m_targetsMutex);
  m_targets.push_back({*vk, mods, regId, request.id, false});
  return {};
}

void WindowsGlobalShortcutBackend::unbindShortcut(const QString &id) {
  std::scoped_lock lock(m_targetsMutex);
  std::erase_if(m_targets, [&](const HookTarget &t) {
    if (t.id != id) return false;
    UnregisterHotKey(nullptr, t.regId);
    return true;
  });
}

void WindowsGlobalShortcutBackend::unbindAll() {
  std::scoped_lock lock(m_targetsMutex);
  for (const auto &target : m_targets) {
    UnregisterHotKey(nullptr, target.regId);
  }
  m_targets.clear();
}
