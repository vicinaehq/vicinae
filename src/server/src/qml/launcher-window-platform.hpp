#pragma once
#include <QtGlobal>

class QQuickWindow;

// Native windowing quirks the launcher window needs handled per platform.
namespace LauncherWindowPlatform {

#ifdef Q_OS_WIN
// Unowned toolwindows lose the foreground fight after virtual desktop switches
// and pick up a taskbar button when recreated; owning them from creation is
// the Flow Launcher setup and fixes both.
void adoptWindows(QQuickWindow *launcher, QQuickWindow *hud);

// The system only grants foreground to the process that last received input;
// an empty synthesized input makes that us (the PowerToys Run trick).
void grantForeground();
#else
inline void adoptWindows(QQuickWindow *, QQuickWindow *) {}
inline void grantForeground() {}
#endif

} // namespace LauncherWindowPlatform
