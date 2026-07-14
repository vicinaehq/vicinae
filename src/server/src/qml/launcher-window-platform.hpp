#pragma once
#include <QtGlobal>

class QQuickWindow;

// Native windowing quirks the launcher window needs handled per platform.
namespace LauncherWindowPlatform {

#ifdef Q_OS_WIN
// The system only grants foreground to the process that last received input;
// an empty synthesized input makes that us (the PowerToys Run trick).
void grantForeground();
#else
inline void grantForeground() {}
#endif

} // namespace LauncherWindowPlatform
