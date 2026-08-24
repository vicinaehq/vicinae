#pragma once
#include <QtGlobal>

class QQuickWindow;
class QWindow;

// Native windowing quirks the launcher window needs handled per platform.
namespace LauncherWindowPlatform {

#ifdef Q_OS_MACOS
// Raises the drag snap overlay above the menu bar and Dock.
void prepareOverlayWindow(QWindow *window);
#else
inline void prepareOverlayWindow(QWindow *) {}
#endif

#ifdef Q_OS_WIN
// The system only grants foreground to the process that last received input;
// an empty synthesized input makes that us (the PowerToys Run trick).
void grantForeground();
// Called right before hiding: the release of keys still held (escape, return,
// the hotkey key) would otherwise leak to the window regaining foreground.
void suppressHeldKeyReleases();
#else
inline void grantForeground() {}
inline void suppressHeldKeyReleases() {}
#endif

} // namespace LauncherWindowPlatform
