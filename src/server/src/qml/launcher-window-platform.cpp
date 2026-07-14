#include "launcher-window-platform.hpp"
#include <windows.h>

namespace LauncherWindowPlatform {

void grantForeground() {
  INPUT input{};
  input.type = INPUT_MOUSE;
  SendInput(1, &input, sizeof(input));
}

} // namespace LauncherWindowPlatform
