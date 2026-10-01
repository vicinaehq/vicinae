#pragma once

namespace vicinae::macos {

void registerLoginItemOnce();
bool isLoginItemSupported();
bool isLoginItemEnabled();
void setLoginItemEnabled(bool enabled);

} // namespace vicinae::macos
