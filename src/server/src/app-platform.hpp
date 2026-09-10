#pragma once
#include <QtGlobal>

namespace AppPlatform {

#ifdef Q_OS_MACOS
// Before QGuiApplication: Qt's Cocoa input context initialises TSM, which must happen in a UI app
// or the panel never gets typing focus (Caps Lock layout switching).
void beforeGuiApplication();
void afterGuiApplication();
#else
inline void beforeGuiApplication() {}
inline void afterGuiApplication() {}
#endif

} // namespace AppPlatform
