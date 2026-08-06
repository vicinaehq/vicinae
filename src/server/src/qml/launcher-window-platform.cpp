#include "launcher-window-platform.hpp"
#include "services/global-shortcuts/windows-global-shortcut-backend.hpp"
#include <windows.h>

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

} // namespace LauncherWindowPlatform
