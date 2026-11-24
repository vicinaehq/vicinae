#include <QEvent>
#include <QGuiApplication>
#include <QTimer>
#include <QWindow>
#include <algorithm>
#include <vector>

#import <AppKit/AppKit.h>

#include "app-platform.hpp"

namespace {

bool isRegularWindow(const QWindow *window) {
  return window && window->type() == Qt::Window && !window->flags().testFlag(Qt::FramelessWindowHint);
}

class AppWindowController : public QObject {
public:
  explicit AppWindowController(QObject *parent) : QObject(parent) {
    m_menuShortcuts.reserve(16);
    qGuiApp->installEventFilter(this);
    connect(qGuiApp, &QGuiApplication::focusWindowChanged, this, [this] { updateMenuShortcuts(); });
    updateMenuShortcuts();
    QTimer::singleShot(0, this, [this] { updateMenuShortcuts(); });
  }

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
          std::ranges::any_of(QGuiApplication::topLevelWindows(),
                              [](QWindow *window) { return window->isVisible() && isRegularWindow(window); });
      const auto policy =
          hasRegularWindow ? NSApplicationActivationPolicyRegular : NSApplicationActivationPolicyAccessory;
      if (NSApp.activationPolicy != policy) [NSApp setActivationPolicy:policy];
      updateMenuShortcuts();
    });
  }

  void updateMenuShortcuts() {
    for (NSMenuItem *topItem in NSApp.mainMenu.itemArray) {
      for (NSMenuItem *item in topItem.submenu.itemArray) {
        if ((item.submenu && item.submenu == NSApp.servicesMenu) || item.keyEquivalent.length == 0) continue;
        if (std::ranges::none_of(m_menuShortcuts, [item](const auto &saved) { return saved.item == item; }))
          m_menuShortcuts.emplace_back(item, item.keyEquivalent, item.keyEquivalentModifierMask);
      }
    }

    auto *window = QGuiApplication::focusWindow();
    while (window && window->type() == Qt::Popup && window->transientParent())
      window = window->transientParent();
    const bool enabled = isRegularWindow(window);
    for (const auto &shortcut : m_menuShortcuts) {
      shortcut.item.keyEquivalent = enabled ? shortcut.key : @"";
      shortcut.item.keyEquivalentModifierMask = enabled ? shortcut.modifiers : 0;
    }
  }

  struct MenuShortcut {
    NSMenuItem *item;
    NSString *key;
    NSEventModifierFlags modifiers;
  };
  std::vector<MenuShortcut> m_menuShortcuts;
  bool m_updatePending = false;
};

} // namespace

namespace AppPlatform {

void beforeGuiApplication() {
  [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyAccessory];
}

void afterGuiApplication() { new AppWindowController(qGuiApp); }

} // namespace AppPlatform
