#include "ui/windows/launcher-window-platform.hpp"
#include "services/global-shortcuts/windows-global-shortcut-backend.hpp"
#include "services/selection/windows-selection-service.hpp"
#include <QQuickWindow>
#include <QTimer>
#include <chrono>
#include <windows.h>
#include <dwmapi.h>

namespace LauncherWindowPlatform {

void grantForeground() {
  INPUT input{};
  input.type = INPUT_MOUSE;
  SendInput(1, &input, sizeof(input));
}

void suppressHeldKeyReleases() {
  for (UINT vk = 0x08; vk <= 0xFE; ++vk) {
    switch (vk) {
    // never eat modifiers
    case VK_SHIFT:
    case VK_CONTROL:
    case VK_MENU:
    case VK_LSHIFT:
    case VK_RSHIFT:
    case VK_LCONTROL:
    case VK_RCONTROL:
    case VK_LMENU:
    case VK_RMENU:
    case VK_LWIN:
    case VK_RWIN:
      continue;
    default:
      break;
    }
    if (GetAsyncKeyState(static_cast<int>(vk)) < 0) { WindowsGlobalShortcutBackend::suppressNextKeyUp(vk); }
  }
}

bool foregroundLent() { return WindowsSelectionService::isLendingForeground(); }

static void setCloaked(WId window, bool cloaked) {
  BOOL const value = cloaked ? TRUE : FALSE;
  // NOLINTBEGIN(performance-no-int-to-ptr)
  auto *hwnd = reinterpret_cast<HWND>(window);
  // NOLINTEND(performance-no-int-to-ptr)
  DwmSetWindowAttribute(hwnd, DWMWA_CLOAK, &value, sizeof(value));
}

void cloak(QQuickWindow *window) { setCloaked(window->winId(), true); }

void uncloakAfterNextFrame(QQuickWindow *window) {
  static constexpr auto UNCLOAK_FALLBACK = std::chrono::milliseconds(250);

  WId const id = window->winId();
  auto uncloak = [id]() { setCloaked(id, false); };

  QObject::connect(window, &QQuickWindow::frameSwapped, window, uncloak,
                   static_cast<Qt::ConnectionType>(Qt::DirectConnection | Qt::SingleShotConnection));
  QTimer::singleShot(UNCLOAK_FALLBACK, window, uncloak);
}

} // namespace LauncherWindowPlatform
