#pragma once
#include <qnamespace.h>
#include <xkbcommon/xkbcommon-keysyms.h>

namespace XKBCommon {
int fromQtKey(Qt::Key key);
Qt::Key toQtKey(int xkbKeyCode);
}; // namespace XKBCommon
