#include "launcher-window-platform.hpp"
#include <QQuickWindow>
#include <windows.h>

namespace LauncherWindowPlatform {

void adoptWindows(QQuickWindow *launcher, QQuickWindow *hud) {
  if (!launcher) return;

  auto *owner = new QWindow;
  owner->setFlags(Qt::Window | Qt::FramelessWindowHint);
  owner->setGeometry(0, 0, 1, 1);
  owner->create();
  launcher->setTransientParent(owner);
  if (hud) hud->setTransientParent(owner);
}

void grantForeground() {
  INPUT input{};
  input.type = INPUT_MOUSE;
  SendInput(1, &input, sizeof(input));
}

} // namespace LauncherWindowPlatform
