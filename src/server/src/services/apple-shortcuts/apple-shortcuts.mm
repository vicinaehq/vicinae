#import <AppKit/AppKit.h>
#import <ScriptingBridge/ScriptingBridge.h>

#include <QCoreApplication>
#include <QtConcurrentRun>
#include <algorithm>

#include "apple-shortcuts.hpp"

@interface VicinaeAppleShortcut : SBObject
@property(copy, readonly) NSString *name;
@property(copy, readonly) id icon;
- (NSString *)id;
- (id)runWithInput:(id)input;
@end

@interface VicinaeShortcutsEvents : SBApplication
- (SBElementArray<VicinaeAppleShortcut *> *)shortcuts;
@end

@interface VicinaeShortcutsErrorHandler : NSObject <SBApplicationDelegate>
@property(strong) NSError *error;
@end

@implementation VicinaeShortcutsErrorHandler
- (id)eventDidFail:(const AppleEvent *)event withError:(NSError *)error {
  if (!self.error) self.error = error;
  return nil;
}
@end

namespace {

QString errorMessage(NSError *error) {
  if (error.code == errAEEventNotPermitted) {
    return QCoreApplication::translate(
        "AppleShortcuts",
        "Allow Vicinae to control Shortcuts Events in System Settings > Privacy & Security > Automation.");
  }
  return QString::fromNSString(error.localizedDescription);
}

QByteArray iconData(id icon) {
  NSData *data = nil;
  if ([icon isKindOfClass:[NSImage class]]) {
    data = [icon TIFFRepresentation];
  } else if ([icon isKindOfClass:[NSAppleEventDescriptor class]]) {
    data = [icon data];
  } else if ([icon isKindOfClass:[NSData class]]) {
    data = icon;
  }
  if (!data) return {};

  NSBitmapImageRep *bitmap = [NSBitmapImageRep imageRepWithData:data];
  NSData *png = [bitmap representationUsingType:NSBitmapImageFileTypePNG properties:@{}];
  if (!png) return {};
  return QByteArray(static_cast<const char *>(png.bytes), png.length);
}

AppleShortcuts::ListResult listShortcuts() {
  @autoreleasepool {
    @try {
      auto *app = (VicinaeShortcutsEvents *)[[SBApplication alloc]
          initWithBundleIdentifier:@"com.apple.shortcuts.events"];
      if (!app) {
        return std::unexpected(
            QCoreApplication::translate("AppleShortcuts", "Shortcuts Events is unavailable."));
      }

      auto *errors = [VicinaeShortcutsErrorHandler new];
      app.delegate = errors;
      app.timeout = 10 * 60; // Apple event timeouts are measured in ticks (60 per second).
      NSArray<VicinaeAppleShortcut *> *shortcuts = [app.shortcuts get];
      if (errors.error) return std::unexpected(errorMessage(errors.error));
      if (!shortcuts) {
        return std::unexpected(
            QCoreApplication::translate("AppleShortcuts", "Could not read Apple Shortcuts."));
      }

      std::vector<AppleShortcuts::Shortcut> result;
      result.reserve(shortcuts.count);
      for (VicinaeAppleShortcut *shortcut in shortcuts) {
        NSString *identifier = shortcut.id;
        NSString *name = shortcut.name;
        if (errors.error) return std::unexpected(errorMessage(errors.error));
        if (!identifier || !name) continue;

        QByteArray icon = iconData(shortcut.icon);
        if (errors.error) return std::unexpected(errorMessage(errors.error));
        result.emplace_back(AppleShortcuts::Shortcut{
            .id = identifier.UTF8String, .name = name.UTF8String, .icon = std::move(icon)});
      }
      std::ranges::sort(result, {}, &AppleShortcuts::Shortcut::id);
      return result;
    } @catch (NSException *exception) { return std::unexpected(QString::fromNSString(exception.reason)); }
  }
}

AppleShortcuts::RunResult runShortcut(const std::string &id) {
  @autoreleasepool {
    @try {
      // Each request needs its own proxy: discovery must continue to wait for replies.
      auto *app = (VicinaeShortcutsEvents *)[[SBApplication alloc]
          initWithBundleIdentifier:@"com.apple.shortcuts.events"];
      if (!app) {
        return std::unexpected(
            QCoreApplication::translate("AppleShortcuts", "Shortcuts Events is unavailable."));
      }

      auto *errors = [VicinaeShortcutsErrorHandler new];
      app.delegate = errors;
      VicinaeAppleShortcut *shortcut =
          [app.shortcuts objectWithID:[NSString stringWithUTF8String:id.c_str()]];
      app.sendMode = kAENoReply | kAECanInteract;
      [shortcut runWithInput:nil];
      if (errors.error) return std::unexpected(errorMessage(errors.error));
      return {};
    } @catch (NSException *exception) { return std::unexpected(QString::fromNSString(exception.reason)); }
  }
}

} // namespace

QFuture<AppleShortcuts::ListResult> AppleShortcuts::list() { return QtConcurrent::run(listShortcuts); }

QFuture<AppleShortcuts::RunResult> AppleShortcuts::run(std::string id) {
  return QtConcurrent::run([id = std::move(id)] { return runShortcut(id); });
}
