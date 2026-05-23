#include "mac-app-runtime.hpp"
#include "services/app-service/app-service.hpp"

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

#include <QPointer>

@interface MacAppRuntimeObserver : NSObject
- (instancetype)initWithRunningCallback:(std::function<void()>)running
                       frontmostCallback:(std::function<void()>)frontmost;
- (void)stop;
@end

@implementation MacAppRuntimeObserver {
  std::function<void()> _running;
  std::function<void()> _frontmost;
}

- (instancetype)initWithRunningCallback:(std::function<void()>)running
                       frontmostCallback:(std::function<void()>)frontmost {
  if ((self = [super init])) {
    _running = std::move(running);
    _frontmost = std::move(frontmost);
    NSNotificationCenter *nc = [[NSWorkspace sharedWorkspace] notificationCenter];
    [nc addObserver:self
           selector:@selector(handleRunning:)
               name:NSWorkspaceDidLaunchApplicationNotification
             object:nil];
    [nc addObserver:self
           selector:@selector(handleRunning:)
               name:NSWorkspaceDidTerminateApplicationNotification
             object:nil];
    [nc addObserver:self
           selector:@selector(handleFrontmost:)
               name:NSWorkspaceDidActivateApplicationNotification
             object:nil];
  }
  return self;
}

- (void)stop {
  [[[NSWorkspace sharedWorkspace] notificationCenter] removeObserver:self];
  _running = nullptr;
  _frontmost = nullptr;
}

- (void)handleRunning:(NSNotification *)note {
  (void)note;
  if (_running) _running();
}

- (void)handleFrontmost:(NSNotification *)note {
  (void)note;
  if (_frontmost) _frontmost();
}

@end

MacAppRuntime::MacAppRuntime(AppService &appService) : m_appService(appService) {
  refreshRunningCache();

  QPointer<MacAppRuntime> self(this);
  m_observer = [[MacAppRuntimeObserver alloc]
      initWithRunningCallback:[self]() {
        if (!self) return;
        self->refreshRunningCache();
        emit self->runningAppsChanged();
      }
            frontmostCallback:[self]() {
              if (self) emit self->frontmostAppChanged();
            }];
}

MacAppRuntime::~MacAppRuntime() {
  [m_observer stop];
  m_observer = nil;
}

void MacAppRuntime::refreshRunningCache() {
  std::unordered_set<QString> ids;
  @autoreleasepool {
    NSArray<NSRunningApplication *> *apps = [[NSWorkspace sharedWorkspace] runningApplications];
    ids.reserve(apps.count);
    for (NSRunningApplication *a in apps) {
      if (a.bundleIdentifier.length > 0) ids.emplace(QString::fromNSString(a.bundleIdentifier));
    }
  }
  m_runningIds = std::move(ids);
}

bool MacAppRuntime::isRunning(const AbstractApplication &app) const {
  return m_runningIds.contains(app.id());
}

bool MacAppRuntime::activate(const AbstractApplication &app) const {
  @autoreleasepool {
    NSString *bundleId = app.id().toNSString();
    if (bundleId.length == 0) return false;
    NSArray<NSRunningApplication *> *matches =
        [NSRunningApplication runningApplicationsWithBundleIdentifier:bundleId];
    if (matches.count == 0) return false;
    return [matches.firstObject activateWithOptions:NSApplicationActivateAllWindows];
  }
}

std::shared_ptr<AbstractApplication> MacAppRuntime::frontmostApp() const {
  QString bundleId;
  @autoreleasepool {
    NSRunningApplication *front = [[NSWorkspace sharedWorkspace] frontmostApplication];
    if (front && front.bundleIdentifier.length > 0) {
      bundleId = QString::fromNSString(front.bundleIdentifier);
    }
  }
  if (bundleId.isEmpty()) return nullptr;
  return m_appService.findById(bundleId);
}
