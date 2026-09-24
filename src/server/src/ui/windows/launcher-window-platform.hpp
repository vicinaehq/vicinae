#pragma once
#include <QtGlobal>
#include <QGuiApplication>
#ifndef Q_OS_MACOS
#include <QCursor>
#include <QQuickItem>
#endif

class QQuickWindow;
class QWindow;
class QQuickItem;

// Native windowing quirks the launcher window needs handled per platform.
namespace LauncherWindowPlatform {

#ifdef Q_OS_MACOS
// Raises the drag snap overlay above the menu bar and Dock.
void prepareOverlayWindow(QWindow *window);
bool isPointerPressOn(QQuickItem *item);
#else
inline void prepareOverlayWindow(QWindow *) {}
inline bool isPointerPressOn(QQuickItem *item) {
  return QGuiApplication::mouseButtons().testFlag(Qt::LeftButton) &&
         item->contains(item->mapFromGlobal(QCursor::pos()));
}
#endif

#ifdef Q_OS_WIN
// The system only grants foreground to the process that last received input;
// an empty synthesized input makes that us (the PowerToys Run trick).
void grantForeground();
// Called right before hiding: the release of keys still held (escape, return,
// the hotkey key) would otherwise leak to the window regaining foreground.
void suppressHeldKeyReleases();
// The selection service lends the foreground to the target app to send it a copy
// chord; the deactivation that causes is not user focus loss.
bool foregroundLent();
#else
inline void grantForeground() {}
inline void suppressHeldKeyReleases() {}
inline bool foregroundLent() { return false; }
#endif

} // namespace LauncherWindowPlatform
