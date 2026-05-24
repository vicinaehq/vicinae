#include "mac-app-runtime.hpp"
#include "services/app-service/app-service.hpp"

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

#include <QPointer>

@interface MacAppRuntimeObserver : NSObject
- (instancetype)initWithTarget:(QPointer<MacAppRuntime>)target;
- (void)stop;
@end

@implementation MacAppRuntimeObserver {
  QPointer<MacAppRuntime> _target;
}

- (instancetype)initWithTarget:(QPointer<MacAppRuntime>)target {
  if ((self = [super init])) {
    _target = std::move(target);
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
  _target.clear();
}

- (void)handleRunning:(NSNotification *)note {
  (void)note;
  if (auto *t = _target.data()) t->onRunningAppsChanged();
}

- (void)handleFrontmost:(NSNotification *)note {
  (void)note;
  if (auto *t = _target.data()) t->onFrontmostAppChanged();
}

@end

MacAppRuntime::MacAppRuntime(AppService &appService) : m_appService(appService) {
  refreshRunningCache();
  m_observer = [[MacAppRuntimeObserver alloc] initWithTarget:QPointer<MacAppRuntime>(this)];
}

MacAppRuntime::~MacAppRuntime() {
  [m_observer stop];
  m_observer = nil;
}

void MacAppRuntime::onRunningAppsChanged() {
  refreshRunningCache();
  emit runningAppsChanged();
}

void MacAppRuntime::onFrontmostAppChanged() { emit frontmostAppChanged(); }

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
