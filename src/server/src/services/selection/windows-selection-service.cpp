#include "windows-selection-service.hpp"
#include <windows.h>
#include <objbase.h>
#include <oleauto.h>
#include <uiautomation.h>
#include <wrl/client.h>
#include "utils/scoped-com.hpp"

using Microsoft::WRL::ComPtr;

namespace {

using Result = AbstractSelectionService::Result;

HWND g_lastForeground = nullptr;

void CALLBACK foregroundChanged(HWINEVENTHOOK, DWORD, HWND hwnd, LONG, LONG, DWORD, DWORD) {
  if (!hwnd) return;
  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  if (pid && pid != GetCurrentProcessId()) g_lastForeground = hwnd;
}

QString textFromPattern(IUIAutomationTextPattern *pattern) {
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

QString textFromElement(IUIAutomationElement *element) {
  ComPtr<IUnknown> unknown;
  if (FAILED(element->GetCurrentPattern(UIA_TextPatternId, &unknown)) || !unknown) return {};

  ComPtr<IUIAutomationTextPattern> pattern;
  if (FAILED(unknown.As(&pattern)) || !pattern) return {};

  return textFromPattern(pattern.Get());
}

QString selectedTextFromWindow(HWND foreground) {
  ScopedCom com;
  ComPtr<IUIAutomation> automation;
  if (FAILED(
          CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&automation))) ||
      !automation) {
    return {};
  }

  // GetFocusedElement would resolve to the launcher; ask the target thread for its focused control
  GUITHREADINFO info{};
  info.cbSize = sizeof(GUITHREADINFO);
  DWORD thread = GetWindowThreadProcessId(foreground, nullptr);
  HWND focus = (thread && GetGUIThreadInfo(thread, &info) && info.hwndFocus) ? info.hwndFocus : foreground;

  ComPtr<IUIAutomationElement> element;
  if (FAILED(automation->ElementFromHandle(focus, &element)) || !element) return {};

  if (QString text = textFromElement(element.Get()); !text.isEmpty()) return text;

  VARIANT focusedValue;
  focusedValue.vt = VT_BOOL;
  focusedValue.boolVal = VARIANT_TRUE;

  ComPtr<IUIAutomationCondition> condition;
  if (FAILED(automation->CreatePropertyCondition(UIA_HasKeyboardFocusPropertyId, focusedValue, &condition)) ||
      !condition) {
    return {};
  }

  ComPtr<IUIAutomationElement> focused;
  if (FAILED(element->FindFirst(TreeScope_Subtree, condition.Get(), &focused)) || !focused) return {};

  return textFromElement(focused.Get());
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
