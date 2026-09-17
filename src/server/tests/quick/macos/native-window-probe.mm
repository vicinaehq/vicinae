#include <QPointer>
#include <QRectF>
#import <AppKit/AppKit.h>
#include "native-window-probe.hpp"

NativeWindowProbe::~NativeWindowProbe() { stop(); }

void NativeWindowProbe::start(QWindow *window) {
  stop();
  m_frames.clear();
  emit framesChanged();
  NSView *view = (__bridge NSView *)reinterpret_cast<void *>(window->winId());
  NSWindow *nativeWindow = view.window;
  QPointer<NativeWindowProbe> weak(this);
  const auto notifications = std::to_array({NSWindowDidMoveNotification, NSWindowDidResizeNotification});
  for (std::size_t index = 0; index < notifications.size(); ++index) {
    id observer = [[NSNotificationCenter defaultCenter]
        addObserverForName:notifications[index]
                    object:nativeWindow
                     queue:nil
                usingBlock:^(NSNotification *) {
                  if (!weak) return;
                  const NSRect frame = nativeWindow.frame;
                  weak->m_frames.emplace_back(
                      QRectF(frame.origin.x, frame.origin.y, frame.size.width, frame.size.height));
                  emit weak->framesChanged();
                }];
    m_observers[index] = (void *)CFBridgingRetain(observer);
  }
}

void NativeWindowProbe::stop() {
  for (void *&observer : m_observers) {
    if (!observer) continue;
    [[NSNotificationCenter defaultCenter] removeObserver:CFBridgingRelease(observer)];
    observer = nullptr;
  }
}
