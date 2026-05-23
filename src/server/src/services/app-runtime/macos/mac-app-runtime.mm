#include "mac-app-runtime.hpp"

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

#include <QPointer>

@interface MacAppRuntimeObserver : NSObject
- (instancetype)initWithCallback:(std::function<void()>)cb;
- (void)stop;
@end

@implementation MacAppRuntimeObserver {
  std::function<void()> _cb;
}

- (instancetype)initWithCallback:(std::function<void()>)cb {
  if ((self = [super init])) {
    _cb = std::move(cb);
    NSNotificationCenter *nc = [[NSWorkspace sharedWorkspace] notificationCenter];
    [nc addObserver:self
           selector:@selector(handleNotification:)
               name:NSWorkspaceDidLaunchApplicationNotification
             object:nil];
    [nc addObserver:self
           selector:@selector(handleNotification:)
               name:NSWorkspaceDidTerminateApplicationNotification
             object:nil];
  }
  return self;
}

- (void)stop {
  [[[NSWorkspace sharedWorkspace] notificationCenter] removeObserver:self];
  _cb = nullptr;
}

- (void)handleNotification:(NSNotification *)note {
  (void)note;
  if (_cb) _cb();
}

@end

MacAppRuntime::MacAppRuntime() {
  QPointer<MacAppRuntime> self(this);
  m_observer = [[MacAppRuntimeObserver alloc] initWithCallback:[self]() {
    if (self) emit self->runningAppsChanged();
  }];
}

MacAppRuntime::~MacAppRuntime() {
  [m_observer stop];
  m_observer = nil;
}

bool MacAppRuntime::isRunning(const AbstractApplication &app) const {
  @autoreleasepool {
    NSString *bundleId = app.id().toNSString();
    if (bundleId.length == 0) return false;
    NSArray<NSRunningApplication *> *matches =
        [NSRunningApplication runningApplicationsWithBundleIdentifier:bundleId];
    return matches.count > 0;
  }
}
