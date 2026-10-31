#include "windows-selection-service.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "services/window-manager/windows/windows-window.hpp"
#include "utils/scoped-com.hpp"
#include <QClipboard>
#include <QGuiApplication>
#include <QMimeData>
#include <QPromise>
#include <QTimer>
#include <QtLogging>
#include <chrono>
#include <memory>
#include <vector>
#include <windows.h>
#include <objbase.h>
#include <oleauto.h>
#include <uiautomation.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace {

using Result = AbstractSelectionService::Result;

constexpr int COPY_POLL_INTERVAL_MS = 10;
constexpr int COPY_TIMEOUT_MS = 1000;
constexpr int HANDOFF_TIMEOUT_MS = 400;
constexpr UINT WM_COPY_TIMEOUT_MS = 50;
constexpr int MAX_ANCESTOR_WALK = 32;
constexpr auto FOREGROUND_LEASE_GRACE = std::chrono::milliseconds(1000);
constexpr ULONG_PTR VICINAE_INJECT_TAG = 0x7669636e; // 'vicn'
constexpr WORD START_MENU_SUPPRESS_VK = 0xFF;
constexpr WORD MODIFIERS[] = {VK_LCONTROL, VK_RCONTROL, VK_LSHIFT, VK_RSHIFT,
                              VK_LMENU,    VK_RMENU,    VK_LWIN,   VK_RWIN};

std::chrono::steady_clock::time_point g_foregroundLentAt{};

QFuture<Result> ready(Result result) { return QtFuture::makeReadyValueFuture<Result>(std::move(result)); }

QFuture<Result> fail(const QString &error) { return ready(std::unexpected(error)); }

Result noSelection() { return std::unexpected(QStringLiteral("Unable to get selected text")); }

bool isOurs(HWND hwnd) {
  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  return pid == GetCurrentProcessId();
}

QString selectionText(IUIAutomationElement *element) {
  ComPtr<IUnknown> unknown;
  if (FAILED(element->GetCurrentPattern(UIA_TextPatternId, &unknown)) || !unknown) return {};

  ComPtr<IUIAutomationTextPattern> pattern;
  if (FAILED(unknown.As(&pattern)) || !pattern) return {};

  ComPtr<IUIAutomationTextRangeArray> ranges;
  if (FAILED(pattern->GetSelection(&ranges)) || !ranges) return {};

  int count = 0;
  ranges->get_Length(&count);

  QString text;
  for (int i = 0; i < count; ++i) {
    ComPtr<IUIAutomationTextRange> range;
    if (FAILED(ranges->GetElement(i, &range)) || !range) continue;

    BSTR bstr = nullptr;
    if (SUCCEEDED(range->GetText(-1, &bstr)) && bstr) {
      text += QString::fromWCharArray(bstr, static_cast<int>(SysStringLen(bstr)));
      SysFreeString(bstr);
    }
  }

  return text;
}

ComPtr<IUIAutomationCondition> boolCondition(IUIAutomation *automation, PROPERTYID property) {
  VARIANT value;
  value.vt = VT_BOOL;
  value.boolVal = VARIANT_TRUE;

  ComPtr<IUIAutomationCondition> condition;
  if (FAILED(automation->CreatePropertyCondition(property, value, &condition))) return nullptr;
  return condition;
}

QString selectionFromAncestry(IUIAutomation *automation, IUIAutomationElement *start) {
  ComPtr<IUIAutomationTreeWalker> walker;
  if (FAILED(automation->get_RawViewWalker(&walker)) || !walker) return selectionText(start);

  ComPtr<IUIAutomationElement> current = start;
  for (int depth = 0; current && depth < MAX_ANCESTOR_WALK; ++depth) {
    if (QString text = selectionText(current.Get()); !text.isEmpty()) return text;

    ComPtr<IUIAutomationElement> parent;
    if (FAILED(walker->GetParentElement(current.Get(), &parent))) break;
    current = parent;
  }

  return {};
}

HWND focusedWindow(HWND foreground) {
  GUITHREADINFO info{};
  info.cbSize = sizeof(GUITHREADINFO);
  DWORD thread = GetWindowThreadProcessId(foreground, nullptr);
  if (thread && GetGUIThreadInfo(thread, &info) && info.hwndFocus) return info.hwndFocus;
  return foreground;
}

// foreground alone is not enough: a browser reports foreground before its content window takes keyboard
// focus, and a chord sent in that gap is dropped
bool targetHasKeyboardFocus(HWND target) {
  if (GetForegroundWindow() != target) return false;
  GUITHREADINFO info{};
  info.cbSize = sizeof(GUITHREADINFO);
  DWORD thread = GetWindowThreadProcessId(target, nullptr);
  return thread && GetGUIThreadInfo(thread, &info) && info.hwndFocus != nullptr;
}

QString uiaSelectedText(HWND focus) {
  ScopedCom com;
  ComPtr<IUIAutomation> automation;
  if (FAILED(
          CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&automation))) ||
      !automation) {
    return {};
  }

  ComPtr<IUIAutomationElement> element;
  if (FAILED(automation->ElementFromHandle(focus, &element)) || !element) return {};

  // GetFocusedElement would resolve to the launcher, not the target
  ComPtr<IUIAutomationElement> start = element;
  if (auto condition = boolCondition(automation.Get(), UIA_HasKeyboardFocusPropertyId)) {
    ComPtr<IUIAutomationElement> deepest;
    if (SUCCEEDED(element->FindFirst(TreeScope_Subtree, condition.Get(), &deepest)) && deepest)
      start = deepest;
  }

  return selectionFromAncestry(automation.Get(), start.Get());
}

INPUT keyInput(WORD vk, bool down) {
  INPUT in{};
  in.type = INPUT_KEYBOARD;
  in.ki.wVk = vk;
  in.ki.wScan = static_cast<WORD>(MapVirtualKeyW(vk, MAPVK_VK_TO_VSC));
  in.ki.dwFlags = down ? 0 : KEYEVENTF_KEYUP;
  in.ki.dwExtraInfo = VICINAE_INJECT_TAG;
  return in;
}

// modifiers still held from the triggering chord would turn ctrl+c into something else
void sendCopyChord() {
  std::vector<INPUT> inputs;
  std::vector<WORD> held;

  for (WORD vk : MODIFIERS) {
    if (GetAsyncKeyState(vk) < 0) {
      held.push_back(vk);
      inputs.push_back(keyInput(vk, false));
    }
  }

  inputs.push_back(keyInput(VK_CONTROL, true));
  inputs.push_back(keyInput('C', true));
  inputs.push_back(keyInput('C', false));
  inputs.push_back(keyInput(VK_CONTROL, false));

  for (WORD vk : held) {
    inputs.push_back(keyInput(vk, true));
  }
  // a re-pressed win key would otherwise open the start menu on its physical release
  inputs.push_back(keyInput(START_MENU_SUPPRESS_VK, false));

  SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
}

HWND lendForeground(HWND target) {
  HWND current = GetForegroundWindow();
  if (current == target) return nullptr;

  g_foregroundLentAt = std::chrono::steady_clock::now();
  SetForegroundWindow(target);
  return isOurs(current) ? current : nullptr;
}

void reclaimForeground(HWND launcher) {
  // the system only grants foreground to the process that last received input
  INPUT input{};
  input.type = INPUT_MOUSE;
  SendInput(1, &input, sizeof(input));
  SetForegroundWindow(launcher);
  g_foregroundLentAt = std::chrono::steady_clock::now();
}

std::unique_ptr<QMimeData> snapshotClipboard() {
  auto copy = std::make_unique<QMimeData>();
  const QMimeData *mime = QGuiApplication::clipboard()->mimeData();
  if (!mime) return copy;

  for (const QString &format : mime->formats()) {
    copy->setData(format, mime->data(format));
  }

  return copy;
}

struct CopyState {
  DWORD sequence = 0;
  std::unique_ptr<QMimeData> snapshot;
  HWND target = nullptr;
  HWND launcher = nullptr;
  int elapsed = 0;
  int readyAt = -1;
  int chordSentAt = -1;
  const char *tier = "wm_copy";
};

} // namespace

WindowsSelectionService::WindowsSelectionService(ClipboardService &clipboard, WindowManager &wm)
    : m_clipboard(clipboard), m_wm(wm) {}

bool WindowsSelectionService::isLendingForeground() {
  return std::chrono::steady_clock::now() - g_foregroundLentAt < FOREGROUND_LEASE_GRACE;
}

QFuture<Result> WindowsSelectionService::selectedText() {
  auto window = m_wm.getFocusedWindow();
  if (!window) {
    qInfo() << "selection: no remembered target window";
    return ready(noSelection());
  }

  HWND target = static_cast<const Win::Window &>(*window).hwnd();
  if (!IsWindow(target)) {
    qInfo() << "selection: remembered window is gone" << window->title();
    return ready(noSelection());
  }
  if (m_copying) return fail(QStringLiteral("A selection capture is already in progress"));

  HWND focus = focusedWindow(target);
  qInfo().nospace() << "selection: target " << window->title() << " (class=" << window->wmClass()
                    << ", focus hwnd=" << focus << ")";

  if (QString text = uiaSelectedText(focus); !text.isEmpty()) {
    qInfo() << "selection: uia hit," << text.size() << "chars";
    return ready(std::move(text));
  }

  return copyFromTarget(target, focus);
}

QFuture<Result> WindowsSelectionService::copyFromTarget(void *target, void *focus) {
  auto state = std::make_shared<CopyState>();
  state->target = static_cast<HWND>(target);
  state->sequence = GetClipboardSequenceNumber();
  state->snapshot = snapshotClipboard();

  // classic edit controls copy on request without needing the foreground
  SendMessageTimeoutW(static_cast<HWND>(focus), WM_COPY, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK,
                      WM_COPY_TIMEOUT_MS, nullptr);

  if (GetClipboardSequenceNumber() != state->sequence) {
    state->chordSentAt = 0;
  } else {
    state->launcher = lendForeground(state->target);
    state->tier = "ctrl_c";
  }

  m_copying = true;

  auto promise = std::make_shared<QPromise<Result>>();
  promise->start();

  auto *timer = new QTimer(this);
  timer->setInterval(COPY_POLL_INTERVAL_MS);

  connect(timer, &QTimer::timeout, this, [this, timer, promise, state]() {
    state->elapsed += COPY_POLL_INTERVAL_MS;

    const auto finish = [&](Result result) {
      timer->deleteLater();
      if (state->launcher) reclaimForeground(state->launcher);
      m_copying = false;
      promise->addResult(std::move(result));
      promise->finish();
    };

    // activation is async: send the chord only once the target actually holds keyboard focus, otherwise it
    // arrives before the app is ready and is dropped
    if (state->chordSentAt < 0) {
      if (state->readyAt < 0) {
        if (targetHasKeyboardFocus(state->target)) {
          state->readyAt = state->elapsed;
        } else if (state->elapsed >= HANDOFF_TIMEOUT_MS) {
          qWarning() << "selection: target never took keyboard focus, foreground" << GetForegroundWindow();
          finish(noSelection());
        }
        return;
      }

      state->sequence = GetClipboardSequenceNumber();
      sendCopyChord();
      state->chordSentAt = state->elapsed;
      return;
    }

    const int sinceChord = state->elapsed - state->chordSentAt;
    const bool timedOut = sinceChord >= COPY_TIMEOUT_MS;

    if (GetClipboardSequenceNumber() == state->sequence) {
      if (!timedOut) return;
      qInfo() << "selection:" << state->tier << "clipboard unchanged after" << sinceChord << "ms";
      finish(noSelection());
      return;
    }

    QString text = QGuiApplication::clipboard()->text();
    if (text.isEmpty() && !timedOut) return;

    // transient so neither we nor the system re-index what was already on the clipboard
    m_clipboard.copyQMimeData(state->snapshot.release(), {.transient = true});

    if (text.isEmpty()) {
      qInfo() << "selection:" << state->tier << "clipboard changed but holds no text after" << sinceChord
              << "ms";
      finish(noSelection());
    } else {
      qInfo() << "selection:" << state->tier << "hit," << text.size() << "chars after" << sinceChord
              << "ms, ready" << state->readyAt << "ms";
      finish(std::move(text));
    }
  });
  timer->start();

  return promise->future();
}
