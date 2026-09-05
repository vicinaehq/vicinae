#include <QtLogging>
#include <future>
#include <windows.h>
#include "services/global-shortcuts/windows-global-shortcut-backend.hpp"

namespace {

constexpr ULONG_PTR VICINAE_INJECT_TAG = 0x7669636e;
constexpr UINT START_MENU_SUPPRESS_VK = 0xFF;
constexpr UINT CHORD_BROKEN = ~0u;

constexpr std::pair<Qt::Key, UINT> KEY_TABLE[] = {
    {Qt::Key_Space, VK_SPACE},       {Qt::Key_Return, VK_RETURN},
    {Qt::Key_Enter, VK_RETURN},      {Qt::Key_Escape, VK_ESCAPE},
    {Qt::Key_Tab, VK_TAB},           {Qt::Key_Backspace, VK_BACK},
    {Qt::Key_Delete, VK_DELETE},     {Qt::Key_Home, VK_HOME},
    {Qt::Key_End, VK_END},           {Qt::Key_PageUp, VK_PRIOR},
    {Qt::Key_PageDown, VK_NEXT},     {Qt::Key_Left, VK_LEFT},
    {Qt::Key_Right, VK_RIGHT},       {Qt::Key_Up, VK_UP},
    {Qt::Key_Down, VK_DOWN},         {Qt::Key_Minus, VK_OEM_MINUS},
    {Qt::Key_Equal, VK_OEM_PLUS},    {Qt::Key_Plus, VK_OEM_PLUS},
    {Qt::Key_BracketLeft, VK_OEM_4}, {Qt::Key_BracketRight, VK_OEM_6},
    {Qt::Key_Backslash, VK_OEM_5},   {Qt::Key_Semicolon, VK_OEM_1},
    {Qt::Key_Apostrophe, VK_OEM_7},  {Qt::Key_Comma, VK_OEM_COMMA},
    {Qt::Key_Period, VK_OEM_PERIOD}, {Qt::Key_Slash, VK_OEM_2},
    {Qt::Key_QuoteLeft, VK_OEM_3},   {Qt::Key_Meta, VK_LWIN},
    {Qt::Key_Meta, VK_RWIN},         {Qt::Key_Control, VK_LCONTROL},
    {Qt::Key_Control, VK_RCONTROL},  {Qt::Key_Alt, VK_LMENU},
    {Qt::Key_Alt, VK_RMENU},         {Qt::Key_Shift, VK_LSHIFT},
    {Qt::Key_Shift, VK_RSHIFT},
};

std::optional<UINT> vkForQtKey(Qt::Key key) {
  if (key >= Qt::Key_A && key <= Qt::Key_Z) { return 'A' + (key - Qt::Key_A); }
  if (key >= Qt::Key_0 && key <= Qt::Key_9) { return '0' + (key - Qt::Key_0); }
  if (key >= Qt::Key_F1 && key <= Qt::Key_F12) { return VK_F1 + (key - Qt::Key_F1); }
  for (const auto &[qt, vk] : KEY_TABLE) {
    if (qt == key) return vk;
  }
  return std::nullopt;
}

Qt::Key qtKeyForVk(UINT vk) {
  if (vk >= 'A' && vk <= 'Z') { return static_cast<Qt::Key>(Qt::Key_A + (vk - 'A')); }
  if (vk >= '0' && vk <= '9') { return static_cast<Qt::Key>(Qt::Key_0 + (vk - '0')); }
  if (vk >= VK_F1 && vk <= VK_F12) { return static_cast<Qt::Key>(Qt::Key_F1 + (vk - VK_F1)); }
  for (const auto &[qt, entry] : KEY_TABLE) {
    if (entry == vk) return qt;
  }
  return Qt::Key_unknown;
}

UINT modBitForVk(UINT vk) {
  switch (vk) {
  case VK_LCONTROL:
  case VK_RCONTROL:
    return MOD_CONTROL;
  case VK_LMENU:
  case VK_RMENU:
    return MOD_ALT;
  case VK_LSHIFT:
  case VK_RSHIFT:
    return MOD_SHIFT;
  case VK_LWIN:
  case VK_RWIN:
    return MOD_WIN;
  default:
    return 0;
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

Qt::KeyboardModifiers qtModifiers(UINT win) {
  Qt::KeyboardModifiers mods;
  if (win & MOD_CONTROL) { mods |= Qt::ControlModifier; }
  if (win & MOD_ALT) { mods |= Qt::AltModifier; }
  if (win & MOD_SHIFT) { mods |= Qt::ShiftModifier; }
  if (win & MOD_WIN) { mods |= Qt::MetaModifier; }
  return mods;
}

WindowsGlobalShortcutBackend *g_backend = nullptr;
UINT g_heldMods = 0;

// One-shot key up suppressions, armed when the launcher hides while keys are still
// physically held (escape, return...). Without this the window regaining foreground
// receives the orphan WM_KEYUP, which some apps act upon.
constexpr ULONGLONG KEY_UP_SUPPRESSION_TTL_MS = 2000;

struct KeyUpSuppression {
  UINT vk;
  ULONGLONG deadline;
};

std::mutex g_suppressionsMutex;
std::vector<KeyUpSuppression> g_suppressions;

// Any event for an armed vk disarms it; only a key up within the deadline is eaten.
// A fresh key down means the next key up belongs to a new press and must go through.
bool consumeSuppressedKeyUp(UINT vk, bool down) {
  std::scoped_lock lock(g_suppressionsMutex);
  const ULONGLONG now = GetTickCount64();
  bool eat = false;
  std::erase_if(g_suppressions, [&](const KeyUpSuppression &s) {
    if (now > s.deadline) return true;
    if (s.vk != vk) return false;
    eat = !down;
    return true;
  });
  return eat;
}

UINT currentModifiers() {
  UINT mods = 0;
  if (GetAsyncKeyState(VK_CONTROL) < 0) mods |= MOD_CONTROL;
  if (GetAsyncKeyState(VK_MENU) < 0) mods |= MOD_ALT;
  if (GetAsyncKeyState(VK_SHIFT) < 0) mods |= MOD_SHIFT;
  if (GetAsyncKeyState(VK_LWIN) < 0 || GetAsyncKeyState(VK_RWIN) < 0) mods |= MOD_WIN;
  return mods;
}

void suppressStartMenu() {
  INPUT inputs[2]{};
  for (auto &input : inputs) {
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = START_MENU_SUPPRESS_VK;
    input.ki.dwExtraInfo = VICINAE_INJECT_TAG;
  }
  inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
  SendInput(2, inputs, sizeof(INPUT));
}

bool isForegroundOurs() {
  DWORD pid = 0;
  GetWindowThreadProcessId(GetForegroundWindow(), &pid);
  return pid == GetCurrentProcessId();
}

// We use a low level keyboard hook because the registered hotkey doesn't fire correctly
// under some circumstances.
// Typically, switching virtual workspaces and then immediately pressing the registered hotkey
// won't work until another shell action is performed (opening a new window, pressing Win+Tab again...)
// The hook works in all cases so we rely on that instead
LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HC_ACTION) {
    const auto *k = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);
    if (k->dwExtraInfo == VICINAE_INJECT_TAG) { return CallNextHookEx(nullptr, nCode, wParam, lParam); }

    const bool down = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    const UINT bit = modBitForVk(k->vkCode);
    if (down) {
      g_heldMods |= bit;
    } else {
      g_heldMods &= ~bit;
    }
    const UINT mods = (currentModifiers() | g_heldMods) & (down ? ~0u : ~bit);
    // both must run on every event so their internal states stay consistent
    const bool shortcutEaten = g_backend && g_backend->dispatchKey(k->vkCode, mods, down);
    const bool suppressionEaten = consumeSuppressedKeyUp(k->vkCode, down);
    if (shortcutEaten || suppressionEaten) { return 1; }
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

void WindowsGlobalShortcutBackend::activate(const QString &id) {
  QMetaObject::invokeMethod(
      this, [this, id]() { emit shortcutActivated(id, GetTickCount64()); }, Qt::QueuedConnection);
}

bool WindowsGlobalShortcutBackend::dispatchKey(unsigned int vk, unsigned int mods, bool down) {
  std::scoped_lock lock(m_targetsMutex);

  if (m_capturing && isForegroundOurs()) {
    const auto key = qtKeyForVk(vk);
    const int qtMods = qtModifiers(mods).toInt();
    QMetaObject::invokeMethod(
        this, [this, key, qtMods, down]() { emit keyCaptured(key, qtMods, down); }, Qt::QueuedConnection);
    return true;
  }

  if (modBitForVk(vk)) { return dispatchModifier(mods, down); }

  bool eaten = false;

  if (down && mods) { m_chord = CHORD_BROKEN; }

  for (auto &target : m_targets) {
    if (target.vk != vk) continue;
    if (!down) {
      // eat the key up of an eaten key down: some apps act on release (e.g. a
      // focused browser button activates on space key up)
      if (target.down) { eaten = true; }
      target.down = false;
      continue;
    }
    if (mods == target.mods) {
      if (!target.down) {
        target.down = true;
        if (mods & MOD_WIN) { suppressStartMenu(); }
        activate(target.id);
      }
      eaten = true;
    }
  }
  return eaten;
}

bool WindowsGlobalShortcutBackend::dispatchModifier(unsigned int mods, bool down) {
  if (down) {
    m_chord |= mods;
    return false;
  }

  for (const auto &target : m_targets) {
    if (!modBitForVk(target.vk) || target.mods != m_chord) continue;
    if (m_chord & MOD_WIN) { suppressStartMenu(); }
    activate(target.id);
    m_chord = CHORD_BROKEN;
    break;
  }

  if (mods == 0) { m_chord = 0; }
  return false;
}

void WindowsGlobalShortcutBackend::setCapturing(bool capturing) {
  std::scoped_lock lock(m_targetsMutex);
  m_capturing = capturing;
  m_chord = 0;
}

void WindowsGlobalShortcutBackend::suppressNextKeyUp(unsigned int vk) {
  std::scoped_lock lock(g_suppressionsMutex);
  std::erase_if(g_suppressions, [&](const KeyUpSuppression &s) { return s.vk == vk; });
  g_suppressions.push_back({vk, GetTickCount64() + KEY_UP_SUPPRESSION_TTL_MS});
}

std::expected<void, QString>
WindowsGlobalShortcutBackend::bindShortcut(const GlobalShortcutRequest &request) {
  unbindShortcut(request.id);

  const auto vk = vkForQtKey(request.trigger.key());

  if (!vk) { return std::unexpected(tr("unsupported or invalid trigger")); }

  const UINT mods = winModifiers(request.trigger.mods());

  if (const UINT bit = modBitForVk(*vk)) {
    std::scoped_lock lock(m_targetsMutex);
    m_targets.push_back({*vk, mods | bit, 0, request.id, false});
    return {};
  }

  const int regId = m_nextRegistrationId++;

  // The hook activates shortcuts; the registration only keeps other apps from claiming the combo.
  const bool registered = RegisterHotKey(nullptr, regId, mods, *vk);

  std::scoped_lock lock(m_targetsMutex);
  m_targets.push_back({*vk, mods, registered ? regId : 0, request.id, false});
  return {};
}

void WindowsGlobalShortcutBackend::unbindShortcut(const QString &id) {
  std::scoped_lock lock(m_targetsMutex);
  std::erase_if(m_targets, [&](const HookTarget &t) {
    if (t.id != id) return false;
    if (t.regId) UnregisterHotKey(nullptr, t.regId);
    return true;
  });
}

void WindowsGlobalShortcutBackend::unbindAll() {
  std::scoped_lock lock(m_targetsMutex);
  for (const auto &target : m_targets) {
    if (target.regId) UnregisterHotKey(nullptr, target.regId);
  }
  m_targets.clear();
}
