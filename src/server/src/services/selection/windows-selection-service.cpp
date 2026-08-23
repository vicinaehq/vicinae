#include "windows-selection-service.hpp"
#include <memory>
#include <windows.h>
#include <uiautomation.h>

namespace {

using Result = AbstractSelectionService::Result;

HWND g_lastForeground = nullptr;

void CALLBACK foregroundChanged(HWINEVENTHOOK, DWORD, HWND hwnd, LONG, LONG, DWORD, DWORD) {
  if (!hwnd) return;
  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  if (pid && pid != GetCurrentProcessId()) g_lastForeground = hwnd;
}

template <typename T> struct ComRelease {
  void operator()(T *p) const {
    if (p) p->Release();
  }
};
template <typename T> using ComPtr = std::unique_ptr<T, ComRelease<T>>;

QString textFromPattern(IUIAutomationTextPattern *pattern) {
  IUIAutomationTextRangeArray *rawRanges = nullptr;
  if (FAILED(pattern->GetSelection(&rawRanges)) || !rawRanges) return {};
  ComPtr<IUIAutomationTextRangeArray> ranges(rawRanges);

  int count = 0;
  ranges->get_Length(&count);

  QString text;
  for (int i = 0; i < count; ++i) {
    IUIAutomationTextRange *rawRange = nullptr;
    if (FAILED(ranges->GetElement(i, &rawRange)) || !rawRange) continue;
    ComPtr<IUIAutomationTextRange> range(rawRange);

    BSTR bstr = nullptr;
    if (SUCCEEDED(range->GetText(-1, &bstr)) && bstr) {
      text += QString::fromWCharArray(bstr, static_cast<int>(SysStringLen(bstr)));
      SysFreeString(bstr);
    }
  }

  return text;
}

QString textFromElement(IUIAutomationElement *element) {
  IUnknown *rawPattern = nullptr;
  if (FAILED(element->GetCurrentPattern(UIA_TextPatternId, &rawPattern)) || !rawPattern) return {};
  ComPtr<IUnknown> unknown(rawPattern);

  IUIAutomationTextPattern *rawText = nullptr;
  if (FAILED(unknown->QueryInterface(IID_PPV_ARGS(&rawText))) || !rawText) return {};
  ComPtr<IUIAutomationTextPattern> pattern(rawText);

  return textFromPattern(pattern.get());
}

QString selectedTextFromWindow(HWND foreground) {
  IUIAutomation *rawAutomation = nullptr;
  if (FAILED(CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(&rawAutomation))) ||
      !rawAutomation) {
    return {};
  }
  ComPtr<IUIAutomation> automation(rawAutomation);

  // GetFocusedElement would resolve to the launcher; ask the target thread for its focused control
  GUITHREADINFO info{};
  info.cbSize = sizeof(GUITHREADINFO);
  DWORD thread = GetWindowThreadProcessId(foreground, nullptr);
  HWND focus = (thread && GetGUIThreadInfo(thread, &info) && info.hwndFocus) ? info.hwndFocus : foreground;

  IUIAutomationElement *rawElement = nullptr;
  if (FAILED(automation->ElementFromHandle(focus, &rawElement)) || !rawElement) return {};
  ComPtr<IUIAutomationElement> element(rawElement);

  if (QString text = textFromElement(element.get()); !text.isEmpty()) return text;

  VARIANT focusedValue;
  focusedValue.vt = VT_BOOL;
  focusedValue.boolVal = VARIANT_TRUE;

  IUIAutomationCondition *rawCondition = nullptr;
  if (FAILED(
          automation->CreatePropertyCondition(UIA_HasKeyboardFocusPropertyId, focusedValue, &rawCondition)) ||
      !rawCondition) {
    return {};
  }
  ComPtr<IUIAutomationCondition> condition(rawCondition);

  IUIAutomationElement *rawFocused = nullptr;
  if (FAILED(element->FindFirst(TreeScope_Subtree, condition.get(), &rawFocused)) || !rawFocused) return {};
  ComPtr<IUIAutomationElement> focused(rawFocused);

  return textFromElement(focused.get());
}

} // namespace

WindowsSelectionService::WindowsSelectionService() {
  g_lastForeground = GetForegroundWindow();
  m_hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, foregroundChanged, 0, 0,
                           WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
}

WindowsSelectionService::~WindowsSelectionService() {
  if (m_hook) UnhookWinEvent(static_cast<HWINEVENTHOOK>(m_hook));
}

QFuture<Result> WindowsSelectionService::selectedText() {
  HWND target = g_lastForeground;

  if (!target || !IsWindow(target)) {
    return QtFuture::makeReadyValueFuture<Result>(
        std::unexpected(QStringLiteral("Unable to get selected text")));
  }

  QString text = selectedTextFromWindow(target);

  if (text.isEmpty()) {
    return QtFuture::makeReadyValueFuture<Result>(
        std::unexpected(QStringLiteral("Unable to get selected text")));
  }

  return QtFuture::makeReadyValueFuture<Result>(std::move(text));
}
