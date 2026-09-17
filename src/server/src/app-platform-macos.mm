#include "app-platform.hpp"

#import <AppKit/AppKit.h>

namespace {

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
  clearMenuShortcuts([NSApp mainMenu]);
  dispatch_async(dispatch_get_main_queue(), ^{ clearMenuShortcuts([NSApp mainMenu]); });
}

} // namespace AppPlatform
