#include <QEvent>
#include <QGuiApplication>
#include <QTimer>
#include <QWindow>
#include <algorithm>

#import <AppKit/AppKit.h>

#include "app-platform.hpp"

namespace {

class DockPresenceController : public QObject {
public:
  explicit DockPresenceController(QObject *parent) : QObject(parent) { qGuiApp->installEventFilter(this); }

protected:
  bool eventFilter(QObject *object, QEvent *event) override {
    if (object->isWindowType() &&
        (event->type() == QEvent::Show || event->type() == QEvent::Hide || event->type() == QEvent::Destroy ||
         event->type() == QEvent::WindowStateChange)) {
      scheduleUpdate();
    }
    return QObject::eventFilter(object, event);
  }

private:
  void scheduleUpdate() {
    if (m_updatePending) return;
    m_updatePending = true;
    QTimer::singleShot(0, this, [this] {
      m_updatePending = false;
      const bool hasRegularWindow =
          std::ranges::any_of(QGuiApplication::topLevelWindows(), [](QWindow *window) {
            return window->isVisible() && window->type() == Qt::Window &&
                   !window->flags().testFlag(Qt::FramelessWindowHint);
          });
      const auto policy =
          hasRegularWindow ? NSApplicationActivationPolicyRegular : NSApplicationActivationPolicyAccessory;
      if (NSApp.activationPolicy != policy) [NSApp setActivationPolicy:policy];
    });
  }

  bool m_updatePending = false;
};

void clearMenuShortcuts(NSMenu *menu) {
  if (!menu) return;
  NSMenu *servicesMenu = [NSApp servicesMenu];
  for (NSMenuItem *topItem in menu.itemArray) {
    NSMenu *submenu = topItem.submenu;
    if (!submenu) continue;
    for (NSMenuItem *item in submenu.itemArray) {
      if (item.submenu == servicesMenu) continue;
      item.keyEquivalent = @"";
      item.keyEquivalentModifierMask = 0;
    }
  }
}

} // namespace

namespace AppPlatform {

void beforeGuiApplication() {
  [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyAccessory];
}

void afterGuiApplication() {
  new DockPresenceController(qGuiApp);
  clearMenuShortcuts([NSApp mainMenu]);
  dispatch_async(dispatch_get_main_queue(), ^{ clearMenuShortcuts([NSApp mainMenu]); });
}

} // namespace AppPlatform
