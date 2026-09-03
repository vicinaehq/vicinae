#include "windows-snippet-server.hpp"
#include <QString>
#include <QTimer>
#include <QtLogging>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <future>
#include <ranges>
#include <span>
#include <thread>
#include <windows.h>

namespace {

constexpr std::size_t MAX_BUFFER_SIZE = 32;
constexpr std::size_t CLIPBOARD_THRESHOLD = 100;
constexpr int KEY_RELEASE_TIMEOUT_MS = 500;
constexpr int KEY_POLL_INTERVAL_MS = 5;

// Stamped on every event we inject via SendInput so the hook skips its own output.
constexpr ULONG_PTR VICINAE_INJECT_TAG = 0x7669636e; // 'vicn'

// ToUnicodeEx flags: bit 2 (0x4) keeps the OS keyboard state untouched so dead-key composition still
// happens in the target app (Win10 1607+); bit 0 (0x1) suppresses the menu-active bell.
constexpr UINT TO_UNICODE_FLAGS = (1u << 2) | 1u;

WindowsSnippetServer *g_snippetServer = nullptr;

bool isWordSeparator(char c) {
  return std::isspace(static_cast<unsigned char>(c)) || std::ispunct(static_cast<unsigned char>(c));
}

bool isCaretMove(unsigned vk) {
  switch (vk) {
  case VK_LEFT:
  case VK_RIGHT:
  case VK_UP:
  case VK_DOWN:
  case VK_HOME:
  case VK_END:
  case VK_PRIOR:
  case VK_NEXT:
  case VK_ESCAPE:
    return true;
  default:
    return false;
  }
}

INPUT vkInput(WORD vk, bool down) {
  INPUT in{};
  in.type = INPUT_KEYBOARD;
  in.ki.wVk = vk;
  in.ki.wScan = static_cast<WORD>(MapVirtualKeyW(vk, MAPVK_VK_TO_VSC));
  in.ki.dwFlags = down ? 0 : KEYEVENTF_KEYUP;
  in.ki.dwExtraInfo = VICINAE_INJECT_TAG;
  return in;
}

// surrogate pairs are sent as two consecutive units and recombined by the target app
INPUT unitInput(char16_t unit, bool down) {
  INPUT in{};
  in.type = INPUT_KEYBOARD;
  in.ki.wScan = unit;
  in.ki.dwFlags = KEYEVENTF_UNICODE | (down ? 0 : KEYEVENTF_KEYUP);
  in.ki.dwExtraInfo = VICINAE_INJECT_TAG;
  return in;
}

void send(std::vector<INPUT> &inputs) {
  if (!inputs.empty()) { SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT)); }
}

void sendPresses(const std::vector<INPUT> &presses) {
  std::vector<INPUT> inputs;
  inputs.reserve(presses.size() * 2);
  for (INPUT in : presses) {
    inputs.push_back(in);
    in.ki.dwFlags |= KEYEVENTF_KEYUP;
    inputs.push_back(in);
  }
  send(inputs);
}

void sendRepeatedVk(WORD vk, unsigned count) { sendPresses(std::vector<INPUT>(count, vkInput(vk, true))); }

void sendText(const std::string &text) {
  const QString qtext = QString::fromStdString(text);
  std::vector<INPUT> presses;
  presses.reserve(qtext.size());
  for (const QChar c : qtext) {
    presses.push_back(unitInput(c.unicode(), true));
  }
  sendPresses(presses);
}

void sendPaste() {
  std::vector<INPUT> inputs = {vkInput(VK_CONTROL, true), vkInput('V', true), vkInput('V', false),
                               vkInput(VK_CONTROL, false)};
  send(inputs);
}

constexpr WORD MODIFIERS[] = {VK_LSHIFT, VK_RSHIFT, VK_LCONTROL, VK_RCONTROL,
                              VK_LMENU,  VK_RMENU,  VK_LWIN,     VK_RWIN};

bool anyHeld(std::span<const WORD> keys) {
  return std::ranges::any_of(keys, [](WORD vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; });
}

void waitUntilReleased(std::span<const WORD> keys) {
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(KEY_RELEASE_TIMEOUT_MS);
  while (anyHeld(keys) && std::chrono::steady_clock::now() < deadline) {
    std::this_thread::sleep_for(std::chrono::milliseconds(KEY_POLL_INTERVAL_MS));
  }
}

// the hook sees the key that fired us before it is posted to the app, so injecting right away can land ahead
// of it. its release is only processed once the press has been delivered.
void waitForTriggerKeyRelease(WORD vk) {
  const WORD keys[] = {vk};
  waitUntilReleased(keys);
}

// still-held shift/AltGr would turn the backspaces into ctrl+backspace and the paste into ctrl+shift+v
void waitForModifierRelease() {
  waitUntilReleased(MODIFIERS);

  std::vector<INPUT> releases;
  for (WORD vk : MODIFIERS) {
    if (GetAsyncKeyState(vk) & 0x8000) { releases.push_back(vkInput(vk, false)); }
  }
  send(releases);
}

// translates a physical key press to the text it produces under the foreground window's layout, and reports
// whether a blocking modifier combo (a real shortcut, not AltGr) was held
std::string translateKey(DWORD vk, DWORD scan, bool &blocking) {
  const auto held = [](int k) { return (GetAsyncKeyState(k) & 0x8000) != 0; };
  const bool ctrl = held(VK_CONTROL);
  const bool alt = held(VK_MENU);
  const bool win = held(VK_LWIN) || held(VK_RWIN);

  // AltGr is Ctrl+Alt and must reach ToUnicodeEx to compose (@ # { [ on AZERTY). plain Ctrl, plain Alt,
  // or any Win press is a shortcut and never contributes text.
  blocking = win || (ctrl != alt);

  BYTE keyState[256] = {0};
  if (held(VK_SHIFT)) { keyState[VK_SHIFT] = 0x80; }
  if (ctrl) {
    keyState[VK_CONTROL] = 0x80;
    keyState[VK_LCONTROL] = 0x80;
  }
  if (alt) {
    keyState[VK_MENU] = 0x80;
    keyState[VK_RMENU] = 0x80;
  }
  if (GetKeyState(VK_CAPITAL) & 0x1) { keyState[VK_CAPITAL] = 0x1; }

  HKL layout = nullptr;
  if (HWND fg = GetForegroundWindow()) { layout = GetKeyboardLayout(GetWindowThreadProcessId(fg, nullptr)); }

  wchar_t buf[8] = {0};
  const int rc =
      ToUnicodeEx(vk, scan, keyState, buf, static_cast<int>(std::size(buf)), TO_UNICODE_FLAGS, layout);
  if (rc <= 0) { return {}; } // 0 = no mapping, -1 = dead key pending

  const QString s = QString::fromWCharArray(buf, rc);
  if (s.isEmpty() || s.at(0).unicode() < 0x20 || s.at(0).unicode() == 0x7f) { return {}; }
  return s.toStdString();
}

LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HC_ACTION && g_snippetServer) {
    const auto *k = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);
    const bool down = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    if (down && k->dwExtraInfo != VICINAE_INJECT_TAG) {
      bool blocking = false;
      const std::string utf8 = translateKey(k->vkCode, k->scanCode, blocking);
      g_snippetServer->onKey(k->vkCode, utf8, blocking);
    }
  }
  return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

} // namespace

WindowsSnippetServer::WindowsSnippetServer() {
  g_snippetServer = this;
  startHookThread();
  QTimer::singleShot(0, this, [this]() { emit ready(); });
}

WindowsSnippetServer::~WindowsSnippetServer() {
  if (m_hookThreadId) { PostThreadMessageW(m_hookThreadId, WM_QUIT, 0, 0); }
  if (m_thread.joinable()) { m_thread.join(); }
  g_snippetServer = nullptr;
}

void WindowsSnippetServer::startHookThread() {
  std::promise<bool> installed;
  auto installedFuture = installed.get_future();

  m_thread = std::thread([this, &installed]() {
    m_hookThreadId = GetCurrentThreadId();
    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, keyboardHookProc, GetModuleHandleW(nullptr), 0);
    m_running = hook != nullptr;
    installed.set_value(hook != nullptr);
    if (!hook) { return; }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {}
    UnhookWindowsHookEx(hook);
    m_running = false;
  });

  if (!installedFuture.get()) {
    qWarning() << "WindowsSnippetServer: failed to install keyboard hook";
    if (m_thread.joinable()) { m_thread.join(); }
    m_hookThreadId = 0;
  }
}

void WindowsSnippetServer::registerSnippet(snippet_gen::CreateSnippetRequest payload) {
  std::lock_guard lock(m_mutex);
  std::erase_if(m_snippets, [&](auto &&s) { return s.trigger == payload.trigger; });
  m_snippets.emplace_back(Snippet{.trigger = payload.trigger, .mode = payload.mode});
  std::ranges::sort(m_snippets, [](auto &&a, auto &&b) { return a.trigger.size() > b.trigger.size(); });
}

void WindowsSnippetServer::unregisterSnippet(std::string_view keyword) {
  std::lock_guard lock(m_mutex);
  std::erase_if(m_snippets, [&](auto &&s) { return s.trigger == keyword; });
}

void WindowsSnippetServer::setKeymap(snippet_gen::LayoutInfo) {
  // the hook resolves the foreground window's layout per keystroke, so nothing to configure
}

void WindowsSnippetServer::resetContext() {
  std::lock_guard lock(m_mutex);
  m_text.clear();
  m_undoTrigger.reset();
}

void WindowsSnippetServer::injectExpand(const std::string &text, unsigned charsToDelete, unsigned, bool,
                                        unsigned cursorLeftMoves, bool viaClipboard) {
  waitForTriggerKeyRelease(static_cast<WORD>(m_triggerVk.load()));
  waitForModifierRelease();
  sendRepeatedVk(VK_BACK, charsToDelete);

  if (viaClipboard) {
    sendPaste();
  } else {
    sendText(text);
  }

  sendRepeatedVk(VK_LEFT, cursorLeftMoves);
}

void WindowsSnippetServer::injectUndo(unsigned backspaceCount, const std::string &trigger) {
  waitForTriggerKeyRelease(VK_BACK);
  waitForModifierRelease();
  sendRepeatedVk(VK_BACK, backspaceCount);
  sendText(trigger);
}

void WindowsSnippetServer::setKeyDelay(int) {}

bool WindowsSnippetServer::usesClipboard(std::size_t expandedLength) const {
  return expandedLength > CLIPBOARD_THRESHOLD;
}

bool WindowsSnippetServer::isRunning() const { return m_running.load(); }

void WindowsSnippetServer::onKey(unsigned vk, const std::string &utf8, bool blockingMods) {
  std::lock_guard lock(m_mutex);

  if (m_undoTrigger) {
    if (vk == VK_BACK && !blockingMods) {
      const std::string trigger = *m_undoTrigger;
      m_undoTrigger.reset();
      QMetaObject::invokeMethod(
          this, [this, trigger]() { emit undoTriggered(trigger); }, Qt::QueuedConnection);
      return;
    }
    m_undoTrigger.reset();
  }

  if (isCaretMove(vk)) {
    m_text.clear();
  } else if (vk == VK_BACK) {
    if (blockingMods) {
      m_text.clear();
    } else if (!m_text.empty()) {
      m_text.pop_back();
    }
  } else if (!blockingMods && !utf8.empty()) {
    m_text.append(utf8);
    if (m_text.size() > MAX_BUFFER_SIZE) { m_text.erase(0, m_text.size() - MAX_BUFFER_SIZE); }
  }

  const bool wordSep = !blockingMods && !utf8.empty() && isWordSeparator(utf8.front());

  for (const auto &snippet : m_snippets) {
    if (snippet.mode == snippet_gen::ExpansionMode::Keydown) {
      if (snippet.trigger.size() > m_text.size()) { continue; }
      if (m_text.ends_with(snippet.trigger)) {
        emitExpansionLocked(snippet, vk);
        break;
      }
    } else if (wordSep) {
      if (snippet.trigger.size() + 1 > m_text.size()) { continue; }
      if (std::string_view(m_text).substr(0, m_text.size() - 1).ends_with(snippet.trigger)) {
        emitExpansionLocked(snippet, vk);
        break;
      }
    }
  }
}

void WindowsSnippetServer::emitExpansionLocked(const Snippet &snippet, unsigned vk) {
  m_triggerVk = vk;
  m_text.clear();

  const std::string trigger = snippet.trigger;
  QMetaObject::invokeMethod(
      this, [this, trigger]() { emit keywordTriggered(trigger); }, Qt::QueuedConnection);
  m_undoTrigger = trigger;
}
