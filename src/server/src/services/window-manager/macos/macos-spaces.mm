#import <AppKit/AppKit.h>
#include <dlfcn.h>
#include <QElapsedTimer>
#include <QTimer>
#include <cstdint>
#include <expected>
#include <memory>
#include "macos-spaces.hpp"

// NOLINTBEGIN(bugprone-reserved-identifier, readability-identifier-naming)
extern "C" AXError _AXUIElementGetWindow(AXUIElementRef element, CGWindowID *identifier);
// NOLINTEND(bugprone-reserved-identifier, readability-identifier-naming)

@protocol VicinaeSpaceMoveOperation <NSObject>
- (instancetype)initWithWindows:(NSArray<NSNumber *> *)windows spaceID:(std::uint64_t)spaceID;
- (void)performWithWMBridgeDelegate;
@end

namespace MacosSpaces {
namespace {

using Result = AbstractWindowManager::WorkspaceChangeResult;
using Direction = AbstractWindowManager::Direction;
using SpaceId = std::uint64_t;
using Connection = std::int32_t;
constexpr int VERIFY_INTERVAL_MS = 50;
constexpr int VERIFY_TIMEOUT_MS = 3000;
constexpr int MISSION_CONTROL_READY_MS = 300;
constexpr int ALL_SPACES_MASK = 7;

// SkyLight is private. Resolve it lazily, check the operation's selectors, and verify every move.
struct SpaceApi {
  void *handle =
      dlopen("/System/Library/PrivateFrameworks/SkyLight.framework/SkyLight", RTLD_LAZY | RTLD_LOCAL);
  Connection (*mainConnection)() = handle ? reinterpret_cast<Connection (*)()>(dlsym(handle,
                                                                                     "SLSMainConnectionID"))
                                          : nullptr;
  CFArrayRef (*copyDisplays)(Connection) =
      handle ? reinterpret_cast<CFArrayRef (*)(Connection)>(dlsym(handle, "SLSCopyManagedDisplaySpaces"))
             : nullptr;
  CFStringRef (*copyActiveDisplay)(Connection) = handle
                                                     ? reinterpret_cast<CFStringRef (*)(Connection)>(dlsym(
                                                           handle, "SLSCopyActiveMenuBarDisplayIdentifier"))
                                                     : nullptr;
  CFArrayRef (*copyWindowSpaces)(Connection, int, CFArrayRef) =
      handle ? reinterpret_cast<CFArrayRef (*)(Connection, int, CFArrayRef)>(dlsym(handle,
                                                                                   "SLSCopySpacesForWindows"))
             : nullptr;
  CFStringRef (*copyDisplayForRect)(Connection,
                                    CGRect) = handle
                                                  ? reinterpret_cast<CFStringRef (*)(Connection, CGRect)>(
                                                        dlsym(handle, "SLSCopyBestManagedDisplayForRect"))
                                                  : nullptr;
  bool (*displayIsAnimating)(Connection, CFStringRef) =
      handle
          ? reinterpret_cast<bool (*)(Connection, CFStringRef)>(dlsym(handle, "SLSManagedDisplayIsAnimating"))
          : nullptr;
  Class operationClass = NSClassFromString(@"SLSBridgedMoveWindowsToManagedSpaceOperation");

  ~SpaceApi() {
    if (handle) dlclose(handle);
  }
};

const SpaceApi &spaceApi() {
  static const SpaceApi API;
  return API;
}

NSArray<NSNumber *> *windowSpaces(CGWindowID window) {
  const auto &api = spaceApi();
  NSArray *const spaces = CFBridgingRelease(
      api.copyWindowSpaces(api.mainConnection(), ALL_SPACES_MASK,
                           (__bridge CFArrayRef) @[ [NSNumber numberWithUnsignedInt:window] ]));
  if (![spaces isKindOfClass:[NSArray class]]) return nil;
  for (id space in spaces) {
    if (![space isKindOfClass:[NSNumber class]]) return nil;
  }
  return spaces;
}

struct Destination {
  SpaceId space;
  NSString *display;
};

std::expected<Destination, Result> adjacentSpace(SpaceId current, Direction direction, bool desktopOnly) {
  const auto &api = spaceApi();
  NSArray *const displays = CFBridgingRelease(api.copyDisplays(api.mainConnection()));
  if (![displays isKindOfClass:[NSArray class]]) return std::unexpected(Result::Failed);

  for (id display in displays) {
    if (![display isKindOfClass:[NSDictionary class]]) continue;
    const id displayId = display[@"Display Identifier"];
    if (![displayId isKindOfClass:[NSString class]]) continue;
    const id spaces = display[@"Spaces"];
    if (![spaces isKindOfClass:[NSArray class]]) continue;
    std::optional<SpaceId> previous;
    bool foundCurrent = false;
    for (id space in spaces) {
      if (![space isKindOfClass:[NSDictionary class]]) continue;
      const id type = space[@"type"];
      const id identifier = space[@"id64"];
      if (![type isKindOfClass:[NSNumber class]] || ![identifier isKindOfClass:[NSNumber class]] ||
          ([type intValue] != 0 && (desktopOnly || [type intValue] != 4)))
        continue;
      const SpaceId candidate = [identifier unsignedLongLongValue];
      if (foundCurrent) return Destination{candidate, displayId};
      if (candidate == current) {
        if (direction == Direction::Previous) {
          if (previous) return Destination{*previous, displayId};
          return std::unexpected(Result::NoAdjacentWorkspace);
        }
        foundCurrent = true;
      }
      previous = candidate;
    }
    if (foundCurrent) return std::unexpected(Result::NoAdjacentWorkspace);
  }
  return std::unexpected(Result::Failed);
}

std::optional<SpaceId> activeSpace() {
  const auto &api = spaceApi();
  NSString *const activeDisplay = CFBridgingRelease(api.copyActiveDisplay(api.mainConnection()));
  NSArray *const displays = CFBridgingRelease(api.copyDisplays(api.mainConnection()));
  if (![activeDisplay isKindOfClass:[NSString class]] || ![displays isKindOfClass:[NSArray class]])
    return std::nullopt;
  for (id display in displays) {
    if (![display isKindOfClass:[NSDictionary class]]) continue;
    // With shared Spaces, SkyLight returns one "Main" display instead of individual display UUIDs.
    if (displays.count != 1 && ![display[@"Display Identifier"] isEqual:activeDisplay]) continue;
    const id current = display[@"Current Space"];
    if (![current isKindOfClass:[NSDictionary class]]) return std::nullopt;
    const id identifier = current[@"id64"];
    if ([identifier isKindOfClass:[NSNumber class]]) return [identifier unsignedLongLongValue];
  }
  return std::nullopt;
}

bool isActive(const Destination &target) {
  const auto &api = spaceApi();
  NSArray *const displays = CFBridgingRelease(api.copyDisplays(api.mainConnection()));
  if (![displays isKindOfClass:[NSArray class]]) return false;
  for (id display in displays) {
    if (![display isKindOfClass:[NSDictionary class]] ||
        ![display[@"Display Identifier"] isEqual:target.display])
      continue;
    const id current = display[@"Current Space"];
    if (![current isKindOfClass:[NSDictionary class]]) return false;
    const id identifier = current[@"id64"];
    return [identifier isKindOfClass:[NSNumber class]] && [identifier unsignedLongLongValue] == target.space;
  }
  return false;
}

id axAttribute(id element, CFStringRef name) {
  if (!element) return nil;
  CFTypeRef value = nullptr;
  if (AXUIElementCopyAttributeValue((__bridge AXUIElementRef)element, name, &value) != kAXErrorSuccess)
    return nil;
  return CFBridgingRelease(value);
}

NSArray *axChildren(id element) {
  const id children = axAttribute(element, kAXChildrenAttribute);
  return [children isKindOfClass:[NSArray class]] ? children : nil;
}

id axChild(id parent, NSString *identifier) {
  for (id child in axChildren(parent)) {
    if ([axAttribute(child, kAXIdentifierAttribute) isEqual:identifier]) return child;
  }
  return nil;
}

id dockElement() {
  NSRunningApplication *const dock =
      [NSRunningApplication runningApplicationsWithBundleIdentifier:@"com.apple.dock"].firstObject;
  if (!dock) return nil;
  const id element = CFBridgingRelease(AXUIElementCreateApplication(dock.processIdentifier));
  AXUIElementSetMessagingTimeout((__bridge AXUIElementRef)element, 0.1f);
  return element;
}

id missionControl() { return axChild(dockElement(), @"mc"); }

void logMissionControlStructure(id element, int depth = 0) {
  if (!element || depth > 5) return;
  const id identifier = axAttribute(element, kAXIdentifierAttribute);
  qDebug() << "Mission Control AX:" << depth << QString::fromNSString([identifier description])
           << QString::fromNSString([axAttribute(element, kAXRoleAttribute) description])
           << "display:" << QString::fromNSString([axAttribute(element, CFSTR("AXDisplayID")) description])
           << "children:" << axChildren(element).count;
  if (depth == 0 || [identifier isEqual:@"mc"] || [identifier isEqual:@"mc.display"] ||
      [identifier isEqual:@"mc.spaces"] || [identifier isEqual:@"mc.spaces.list"]) {
    for (id child in axChildren(element))
      logMissionControlStructure(child, depth + 1);
  }
}

bool toggleMissionControl() {
  return [NSWorkspace.sharedWorkspace
      openURL:[NSURL fileURLWithPath:@"/System/Applications/Mission Control.app"]];
}

id spaceButton(id mission, const Destination &target) {
  const auto &api = spaceApi();
  id list = nil;
  for (id display in axChildren(mission)) {
    if (![axAttribute(display, kAXIdentifierAttribute) isEqual:@"mc.display"]) continue;
    const id displayId = axAttribute(display, CFSTR("AXDisplayID"));
    if (![displayId isKindOfClass:[NSNumber class]]) continue;
    const auto identifier = [displayId unsignedIntValue];
    if ([target.display isEqualToString:@"Main"]) {
      if (identifier != CGMainDisplayID()) continue;
    } else {
      NSString *const uuid =
          CFBridgingRelease(api.copyDisplayForRect(api.mainConnection(), CGDisplayBounds(identifier)));
      if (![uuid isEqual:target.display]) continue;
    }
    list = axChild(axChild(display, @"mc.spaces"), @"mc.spaces.list");
    break;
  }
  NSArray *const buttons = axChildren(list);
  if (!buttons) return nil;
  NSArray *const displays = CFBridgingRelease(api.copyDisplays(api.mainConnection()));
  if (![displays isKindOfClass:[NSArray class]]) return nil;
  for (id display in displays) {
    if (![display isKindOfClass:[NSDictionary class]] ||
        ![display[@"Display Identifier"] isEqual:target.display])
      continue;
    const id spaces = display[@"Spaces"];
    // Read the order again after Mission Control opens; it can change while the command is pending.
    if (![spaces isKindOfClass:[NSArray class]] || [spaces count] != buttons.count) return nil;
    NSUInteger index = 0;
    for (id space in spaces) {
      if (![space isKindOfClass:[NSDictionary class]]) return nil;
      const id identifier = space[@"id64"];
      if (![identifier isKindOfClass:[NSNumber class]]) return nil;
      if ([identifier unsignedLongLongValue] == target.space) {
        const id button = [buttons objectAtIndex:index];
        return [axAttribute(button, kAXRoleAttribute) isEqual:(__bridge NSString *)kAXButtonRole] ? button
                                                                                                  : nil;
      }
      ++index;
    }
  }
  return nil;
}

struct PendingChange {
  QPromise<Result> promise;
  QElapsedTimer elapsed;
  QElapsedTimer missionControlReady;
  id<VicinaeSpaceMoveOperation> operation;
  id observer;
  bool switchRequested = false;
  bool openedMissionControl = false;
  bool pressedSpace = false;
  bool spaceChanged = false;

  ~PendingChange() {
    if (observer) [NSWorkspace.sharedWorkspace.notificationCenter removeObserver:observer];
  }
};

QFuture<Result> verifyChange(const Destination &target, std::optional<CGWindowID> window,
                             id<VicinaeSpaceMoveOperation> operation, QObject *context) {
  auto pending = std::make_shared<PendingChange>();
  pending->operation = operation;
  pending->promise.start();
  pending->elapsed.start();
  const std::weak_ptr<PendingChange> weakPending = pending;
  pending->observer = [NSWorkspace.sharedWorkspace.notificationCenter
      addObserverForName:NSWorkspaceActiveSpaceDidChangeNotification
                  object:nil
                   queue:NSOperationQueue.mainQueue
              usingBlock:^(NSNotification *) {
                if (const auto state = weakPending.lock(); state && state->pressedSpace)
                  state->spaceChanged = true;
              }];
  auto future = pending->promise.future();
  auto *timer = new QTimer(context);
  timer->setInterval(VERIFY_INTERVAL_MS);
  QObject::connect(timer, &QTimer::timeout, timer, [pending, timer, window, target]() {
    @autoreleasepool {
      bool moved = true;
      if (window) {
        NSArray<NSNumber *> *const spaces = windowSpaces(*window);
        moved = spaces.count == 1 && spaces.firstObject.unsignedLongLongValue == target.space;
      }
      const id mission = missionControl();
      bool failed = false;
      if (moved && !pending->switchRequested) {
        pending->switchRequested = true;
        pending->elapsed.restart();
        if (!mission) {
          pending->openedMissionControl = toggleMissionControl();
          failed = !pending->openedMissionControl;
        }
      }
      if (moved && mission && !pending->pressedSpace) {
        if (!pending->missionControlReady.isValid()) pending->missionControlReady.start();
        if (pending->missionControlReady.elapsed() >= MISSION_CONTROL_READY_MS) {
          if (const id button = spaceButton(mission, target)) {
            pending->pressedSpace = true;
            pending->elapsed.restart();
            failed =
                AXUIElementPerformAction((__bridge AXUIElementRef)button, kAXPressAction) != kAXErrorSuccess;
          }
        }
      }
      const auto &api = spaceApi();
      const bool animating =
          api.displayIsAnimating &&
          api.displayIsAnimating(api.mainConnection(), (__bridge CFStringRef)target.display);
      // A changed SkyLight ID alone is insufficient: Dock must complete a real Space transition.
      const bool followed = moved && pending->pressedSpace && pending->spaceChanged && !mission &&
                            !animating && isActive(target);
      if (!failed && !followed && pending->elapsed.elapsed() < VERIFY_TIMEOUT_MS) return;
      if (!followed) {
        qDebug() << "Space selection failed: display:" << QString::fromNSString(target.display)
                 << "mission:" << (mission != nil) << "pressed:" << pending->pressedSpace
                 << "animating:" << animating;
        logMissionControlStructure(dockElement());
      }
      if (!followed && mission && pending->openedMissionControl) toggleMissionControl();
      qDebug() << "Space transition completed: destination:" << target.space
               << "window:" << window.value_or(0) << "workspace notification:" << pending->spaceChanged
               << "confirmed:" << followed;
      pending->promise.addResult(followed          ? Result::Success
                                 : window && moved ? Result::FollowFailed
                                                   : Result::Failed);
      pending->promise.finish();
      timer->stop();
      timer->deleteLater();
    }
  });
  timer->start();
  return future;
}

} // namespace

bool supportsWindowMove() {
  // Earlier releases expose similar symbols but can silently reject moves of other apps' windows.
  if (@available(macOS 26.4, *)) {
    const auto &api = spaceApi();
    return api.mainConnection && api.copyDisplays && api.copyWindowSpaces && api.operationClass &&
           api.copyDisplayForRect &&
           [api.operationClass instancesRespondToSelector:@selector(initWithWindows:spaceID:)] &&
           [api.operationClass instancesRespondToSelector:@selector(performWithWMBridgeDelegate)];
  }
  return false;
}

bool supportsSpaceSwitch() {
  const auto &api = spaceApi();
  return api.mainConnection && api.copyDisplays && api.copyActiveDisplay && api.copyDisplayForRect;
}

QFuture<Result> switchSpace(Direction direction, QObject *context) {
  if (!supportsSpaceSwitch()) return QtFuture::makeReadyValueFuture(Result::Unsupported);
  if (!AXIsProcessTrusted()) return QtFuture::makeReadyValueFuture(Result::PermissionRequired);
  @autoreleasepool {
    const auto current = activeSpace();
    if (!current) return QtFuture::makeReadyValueFuture(Result::Failed);
    const auto target = adjacentSpace(*current, direction, false);
    if (!target) return QtFuture::makeReadyValueFuture(target.error());
    return verifyChange(*target, std::nullopt, nil, context);
  }
}

QFuture<Result> moveWindow(AXUIElementRef window, Direction direction, QObject *context) {
  if (!supportsWindowMove()) return QtFuture::makeReadyValueFuture(Result::Unsupported);
  if (!AXIsProcessTrusted()) return QtFuture::makeReadyValueFuture(Result::PermissionRequired);
  CGWindowID identifier = 0;
  if (_AXUIElementGetWindow(window, &identifier) != kAXErrorSuccess || !identifier) {
    return QtFuture::makeReadyValueFuture(Result::Failed);
  }

  @autoreleasepool {
    NSArray<NSNumber *> *const source = windowSpaces(identifier);
    if (!source || source.count == 0) return QtFuture::makeReadyValueFuture(Result::Failed);
    if (source.count != 1) return QtFuture::makeReadyValueFuture(Result::Unsupported);
    const auto target = adjacentSpace(source.firstObject.unsignedLongLongValue, direction, true);
    if (!target) return QtFuture::makeReadyValueFuture(target.error());

    @try {
      const id<VicinaeSpaceMoveOperation> operation =
          [[spaceApi().operationClass alloc] initWithWindows:@[ [NSNumber numberWithUnsignedInt:identifier] ]
                                                     spaceID:target->space];
      if (!operation) return QtFuture::makeReadyValueFuture(Result::Failed);
      [operation performWithWMBridgeDelegate];
      return verifyChange(*target, identifier, operation, context);
    } @catch (NSException *exception) {
      qWarning() << "Space move failed:" << QString::fromNSString(exception.reason);
      return QtFuture::makeReadyValueFuture(Result::Failed);
    }
  }
}

} // namespace MacosSpaces
