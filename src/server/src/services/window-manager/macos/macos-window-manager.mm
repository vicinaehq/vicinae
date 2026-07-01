#include "macos-window-manager.hpp"

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

#include <QFutureWatcher>
#include <QPointer>
#include <QTimer>
#include <QtConcurrent>
#include <array>
#include <chrono>
#include <cstring>
#include <unistd.h>
#include <unordered_set>

#include "macos-window.hpp"

// Private but stable HIServices APIs used by virtually every macOS window switcher (AltTab, HyperSwitch, ...).
// _AXUIElementGetWindow maps an AX window element to its CoreGraphics window id, giving us a stable identity.
// _AXUIElementCreateWithRemoteToken builds an AX element straight from a (pid, element id) token, which lets us
// reach windows living on other Spaces: kAXWindowsAttribute only ever returns windows of the current Space.
// Both degrade gracefully (failed calls are simply skipped) if Apple ever changes them.
extern "C" {
AXError _AXUIElementGetWindow(AXUIElementRef element, CGWindowID *identifier);
AXUIElementRef _AXUIElementCreateWithRemoteToken(CFDataRef token);
}

@interface MacosWindowObserver : NSObject
- (instancetype)initWithTarget:(QPointer<MacosWindowManager>)target;
- (void)stop;
@end

@implementation MacosWindowObserver {
  QPointer<MacosWindowManager> _target;
}

- (instancetype)initWithTarget:(QPointer<MacosWindowManager>)target {
  if ((self = [super init])) {
    _target = std::move(target);
    NSNotificationCenter *nc = [[NSWorkspace sharedWorkspace] notificationCenter];
    for (NSString *name in @[
           NSWorkspaceDidLaunchApplicationNotification,
           NSWorkspaceDidTerminateApplicationNotification,
           NSWorkspaceDidHideApplicationNotification,
           NSWorkspaceDidUnhideApplicationNotification,
         ]) {
      [nc addObserver:self selector:@selector(handleWindowsChanged:) name:name object:nil];
    }
    [nc addObserver:self
           selector:@selector(handleFocusChanged:)
               name:NSWorkspaceDidActivateApplicationNotification
             object:nil];
  }
  return self;
}

- (void)stop {
  [[[NSWorkspace sharedWorkspace] notificationCenter] removeObserver:self];
  _target.clear();
}

- (void)handleWindowsChanged:(NSNotification *)note {
  (void)note;
  if (auto *t = _target.data()) t->notifyWindowsChanged();
}

- (void)handleFocusChanged:(NSNotification *)note {
  (void)note;
  if (auto *t = _target.data()) t->notifyFocusChanged();
}

@end

namespace {

QString axCopyString(AXUIElementRef element, CFStringRef attribute) {
  CFTypeRef value = nullptr;
  if (AXUIElementCopyAttributeValue(element, attribute, &value) != kAXErrorSuccess || !value) return {};
  QString result;
  if (CFGetTypeID(value) == CFStringGetTypeID()) {
    result = QString::fromCFString(static_cast<CFStringRef>(value));
  }
  CFRelease(value);
  return result;
}

bool axHasAttribute(AXUIElementRef element, CFStringRef attribute) {
  CFTypeRef value = nullptr;
  if (AXUIElementCopyAttributeValue(element, attribute, &value) != kAXErrorSuccess || !value) return false;
  CFRelease(value);
  return true;
}

bool axCopyBool(AXUIElementRef element, CFStringRef attribute) {
  CFTypeRef value = nullptr;
  if (AXUIElementCopyAttributeValue(element, attribute, &value) != kAXErrorSuccess || !value) return false;
  bool result = CFGetTypeID(value) == CFBooleanGetTypeID() &&
                CFBooleanGetValue(static_cast<CFBooleanRef>(value));
  CFRelease(value);
  return result;
}

bool axIsSettable(AXUIElementRef element, CFStringRef attribute) {
  Boolean settable = false;
  return AXUIElementIsAttributeSettable(element, attribute, &settable) == kAXErrorSuccess && settable;
}

CFStringRef const kAXFullScreenAttributeName = CFSTR("AXFullScreen");

std::optional<AbstractWindowManager::WindowBounds> axCopyBounds(AXUIElementRef element) {
  CFTypeRef positionValue = nullptr;
  CFTypeRef sizeValue = nullptr;
  CGPoint position{};
  CGSize size{};

  bool hasPosition = AXUIElementCopyAttributeValue(element, kAXPositionAttribute, &positionValue) ==
                         kAXErrorSuccess &&
                     positionValue &&
                     AXValueGetValue(static_cast<AXValueRef>(positionValue), kAXValueTypeCGPoint, &position);
  bool hasSize = AXUIElementCopyAttributeValue(element, kAXSizeAttribute, &sizeValue) == kAXErrorSuccess &&
                 sizeValue && AXValueGetValue(static_cast<AXValueRef>(sizeValue), kAXValueTypeCGSize, &size);

  if (positionValue) CFRelease(positionValue);
  if (sizeValue) CFRelease(sizeValue);

  if (!hasPosition || !hasSize) return std::nullopt;

  return AbstractWindowManager::WindowBounds{.x = static_cast<int32_t>(position.x),
                                             .y = static_cast<int32_t>(position.y),
                                             .width = static_cast<int32_t>(size.width),
                                             .height = static_cast<int32_t>(size.height)};
}

bool isWindowLike(AXUIElementRef element) {
  QString subrole = axCopyString(element, kAXSubroleAttribute);
  if (subrole.isEmpty()) return true;
  return subrole == QString::fromCFString(kAXStandardWindowSubrole) ||
         subrole == QString::fromCFString(kAXDialogSubrole);
}

// Stricter than isWindowLike: brute-forced element ids resolve to all kinds of UI elements (buttons, menus,
// ...), so here we require an explicit window subrole and reject anything without one.
bool hasWindowSubrole(AXUIElementRef element) {
  QString subrole = axCopyString(element, kAXSubroleAttribute);
  return subrole == QString::fromCFString(kAXStandardWindowSubrole) ||
         subrole == QString::fromCFString(kAXDialogSubrole);
}

AbstractWindowManager::WindowPtr buildWindow(AXUIElementRef element, pid_t pid, const QString &bundleId,
                                             const QString &appName) {
  if (!isWindowLike(element)) return nullptr;

  QString title = axCopyString(element, kAXTitleAttribute);
  if (title.isEmpty()) title = appName;

  QString id;
  CGWindowID windowId = 0;
  if (_AXUIElementGetWindow(element, &windowId) == kAXErrorSuccess && windowId != 0) {
    id = QString::number(windowId);
  } else {
    id = QString("%1:%2").arg(bundleId, title);
  }

  bool canClose = axHasAttribute(element, kAXCloseButtonAttribute);
  bool fullScreen = axCopyBool(element, kAXFullScreenAttributeName);
  bool canFullScreen = axIsSettable(element, kAXFullScreenAttributeName);

  return std::make_shared<MacosWindow>(element, std::move(id), std::move(title), bundleId, pid,
                                       axCopyBounds(element), canClose, fullScreen, canFullScreen);
}

const MacosWindow *asMacosWindow(const AbstractWindowManager::AbstractWindow &window) {
  return dynamic_cast<const MacosWindow *>(&window);
}

void appendWindowDeduped(AXUIElementRef element, pid_t pid, const QString &bundleId, const QString &appName,
                         AbstractWindowManager::WindowList &out, std::unordered_set<CGWindowID> &seen) {
  CGWindowID windowId = 0;
  if (_AXUIElementGetWindow(element, &windowId) == kAXErrorSuccess && windowId != 0) {
    if (!seen.insert(windowId).second) return;
  }
  if (auto window = buildWindow(element, pid, bundleId, appName)) { out.emplace_back(std::move(window)); }
}

// Windows on the current Space, as reported by the AX windows attribute. Elements are owned by the returned
// array, so callers must not release them individually (MacosWindow retains the ones it keeps).
void collectCurrentSpaceWindows(pid_t pid, const QString &bundleId, const QString &appName,
                                AbstractWindowManager::WindowList &out,
                                std::unordered_set<CGWindowID> &seen) {
  AXUIElementRef appElement = AXUIElementCreateApplication(pid);
  CFArrayRef axWindows = nullptr;
  if (AXUIElementCopyAttributeValue(appElement, kAXWindowsAttribute,
                                    reinterpret_cast<CFTypeRef *>(&axWindows)) == kAXErrorSuccess &&
      axWindows) {
    CFIndex count = CFArrayGetCount(axWindows);
    out.reserve(out.size() + count);
    for (CFIndex i = 0; i < count; ++i) {
      auto element = static_cast<AXUIElementRef>(CFArrayGetValueAtIndex(axWindows, i));
      appendWindowDeduped(element, pid, bundleId, appName, out, seen);
    }
    CFRelease(axWindows);
  }
  CFRelease(appElement);
}

constexpr uint64_t BRUTE_FORCE_MAX_ID = 1000;
constexpr int BRUTE_FORCE_TOTAL_BUDGET_MS = 500;
constexpr int32_t REMOTE_TOKEN_MAGIC = 0x636f636f;
constexpr int REBUILD_DEBOUNCE_MS = 150;
constexpr int COALESCE_DEBOUNCE_MS = 2500;

void collectOtherSpaceWindows(pid_t pid, const QString &bundleId, const QString &appName,
                              AbstractWindowManager::WindowList &out, std::unordered_set<CGWindowID> &seen,
                              std::chrono::steady_clock::time_point deadline) {
  std::array<uint8_t, 20> token{};
  std::memcpy(token.data(), &pid, sizeof(pid));
  std::memcpy(token.data() + 8, &REMOTE_TOKEN_MAGIC, sizeof(REMOTE_TOKEN_MAGIC));

  for (uint64_t id = 0; id < BRUTE_FORCE_MAX_ID; ++id) {
    if (std::chrono::steady_clock::now() >= deadline) break;

    std::memcpy(token.data() + 12, &id, sizeof(id));
    CFDataRef data = CFDataCreate(kCFAllocatorDefault, token.data(), token.size());
    AXUIElementRef element = _AXUIElementCreateWithRemoteToken(data);
    CFRelease(data);
    if (!element) continue;

    if (hasWindowSubrole(element)) { appendWindowDeduped(element, pid, bundleId, appName, out, seen); }
    CFRelease(element);
  }
}

struct AppDescriptor {
  pid_t pid;
  QString bundleId;
  QString appName;
};

// Must run on the main thread: NSWorkspace is not safe to query off-main. We only collect lightweight
// descriptors here so the expensive AX walk can happen on a worker thread.
std::vector<AppDescriptor> runningRegularApps() {
  std::vector<AppDescriptor> result;
  pid_t selfPid = getpid();

  @autoreleasepool {
    NSArray<NSRunningApplication *> *apps = [[NSWorkspace sharedWorkspace] runningApplications];
    result.reserve(apps.count);
    for (NSRunningApplication *app in apps) {
      if (app.activationPolicy != NSApplicationActivationPolicyRegular) continue;

      pid_t pid = app.processIdentifier;
      if (pid <= 0 || pid == selfPid) continue;

      QString bundleId = app.bundleIdentifier ? QString::fromNSString(app.bundleIdentifier) : QString();
      QString appName = app.localizedName ? QString::fromNSString(app.localizedName) : bundleId;
      result.emplace_back(AppDescriptor{pid, std::move(bundleId), std::move(appName)});
    }
  }

  return result;
}

// Runs on a worker thread: only the (thread-safe) AX API is touched here, no AppKit.
AbstractWindowManager::WindowList scanWindows(const std::vector<AppDescriptor> &apps) {
  AbstractWindowManager::WindowList windows;
  std::unordered_set<CGWindowID> seen;

  auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(BRUTE_FORCE_TOTAL_BUDGET_MS);

  @autoreleasepool {
    for (const auto &app : apps) {
      collectCurrentSpaceWindows(app.pid, app.bundleId, app.appName, windows, seen);
      collectOtherSpaceWindows(app.pid, app.bundleId, app.appName, windows, seen, deadline);
    }
  }

  return windows;
}

} // namespace

MacosWindowManager::MacosWindowManager() = default;

MacosWindowManager::~MacosWindowManager() {
  [m_observer stop];
  m_observer = nil;
}

bool MacosWindowManager::isActivatable() const { return true; }

void MacosWindowManager::start() {
  m_observer = [[MacosWindowObserver alloc] initWithTarget:QPointer<MacosWindowManager>(this)];

  m_rebuildTimer = new QTimer(this);
  m_rebuildTimer->setSingleShot(true);
  m_rebuildTimer->setInterval(REBUILD_DEBOUNCE_MS);
  connect(m_rebuildTimer, &QTimer::timeout, this, &MacosWindowManager::rebuildCache);

  m_coalesceTimer = new QTimer(this);
  m_coalesceTimer->setSingleShot(true);
  m_coalesceTimer->setInterval(COALESCE_DEBOUNCE_MS);
  connect(m_coalesceTimer, &QTimer::timeout, this, &MacosWindowManager::rebuildCache);

  rebuildCache();
}

void MacosWindowManager::refresh() const { scheduleRebuild(); }

void MacosWindowManager::scheduleRebuild() const {
  if (m_rebuildTimer) m_rebuildTimer->start();
}

void MacosWindowManager::scheduleCoalescedRebuild() const {
  if (m_coalesceTimer) m_coalesceTimer->start();
}

// The full AX scan is expensive (the per-app brute-force needed for other-Space windows is cross-process and
// budgeted), so it runs on a worker thread and the result is swapped into the cache on the main thread. Only
// one scan runs at a time; requests arriving mid-scan are coalesced into a single follow-up.
void MacosWindowManager::rebuildCache() {
  if (!AXIsProcessTrusted()) {
    m_cache.clear();
    emit windowsChanged();
    return;
  }

  if (m_rebuilding) {
    m_rebuildPending = true;
    return;
  }
  m_rebuilding = true;
  if (m_coalesceTimer) m_coalesceTimer->stop();

  auto apps = runningRegularApps();
  auto *watcher = new QFutureWatcher<WindowList>(this);
  connect(watcher, &QFutureWatcher<WindowList>::finished, this, [this, watcher]() {
    m_cache = watcher->result();
    watcher->deleteLater();
    m_rebuilding = false;
    emit windowsChanged();
    if (m_rebuildPending) {
      m_rebuildPending = false;
      rebuildCache();
    }
  });

  watcher->setFuture(QtConcurrent::run([apps = std::move(apps)]() { return scanWindows(apps); }));
}

AbstractWindowManager::WindowList MacosWindowManager::listWindowsSync() const { return m_cache; }

AbstractWindowManager::WindowPtr MacosWindowManager::getFocusedWindowSync() const {
  if (!AXIsProcessTrusted()) return nullptr;

  WindowPtr result;

  @autoreleasepool {
    NSRunningApplication *front = [[NSWorkspace sharedWorkspace] frontmostApplication];
    if (!front) return nullptr;

    pid_t pid = front.processIdentifier;
    if (pid <= 0 || pid == getpid()) return nullptr;

    QString bundleId =
        front.bundleIdentifier ? QString::fromNSString(front.bundleIdentifier) : QString();
    QString appName = front.localizedName ? QString::fromNSString(front.localizedName) : bundleId;

    AXUIElementRef appElement = AXUIElementCreateApplication(pid);
    AXUIElementRef focused = nullptr;
    if (AXUIElementCopyAttributeValue(appElement, kAXFocusedWindowAttribute,
                                      reinterpret_cast<CFTypeRef *>(&focused)) == kAXErrorSuccess &&
        focused) {
      result = buildWindow(focused, pid, bundleId, appName);
      CFRelease(focused);
    }
    CFRelease(appElement);
  }

  return result;
}

void MacosWindowManager::focusWindowSync(const AbstractWindow &window) const {
  const MacosWindow *macWindow = asMacosWindow(window);
  if (!macWindow) return;

  AXUIElementRef element = macWindow->element();

  AXUIElementSetAttributeValue(element, kAXMinimizedAttribute, kCFBooleanFalse);
  AXUIElementSetAttributeValue(element, kAXMainAttribute, kCFBooleanTrue);
  AXUIElementPerformAction(element, kAXRaiseAction);

  if (auto pid = macWindow->pid()) {
    @autoreleasepool {
      NSRunningApplication *app =
          [NSRunningApplication runningApplicationWithProcessIdentifier:*pid];
      [app activateWithOptions:NSApplicationActivateAllWindows];
    }
  }
}

bool MacosWindowManager::closeWindow(const AbstractWindow &window) const {
  const MacosWindow *macWindow = asMacosWindow(window);
  if (!macWindow) return false;

  CFTypeRef closeButton = nullptr;
  if (AXUIElementCopyAttributeValue(macWindow->element(), kAXCloseButtonAttribute, &closeButton) !=
          kAXErrorSuccess ||
      !closeButton) {
    return false;
  }

  AXError err = AXUIElementPerformAction(static_cast<AXUIElementRef>(closeButton), kAXPressAction);
  CFRelease(closeButton);

  if (err == kAXErrorSuccess) scheduleRebuild();

  return err == kAXErrorSuccess;
}

bool MacosWindowManager::setWindowBounds(const AbstractWindow &window, const WindowBounds &bounds) const {
  const MacosWindow *macWindow = asMacosWindow(window);
  if (!macWindow) return false;

  AXUIElementRef element = macWindow->element();

  CGPoint position{.x = static_cast<CGFloat>(bounds.x), .y = static_cast<CGFloat>(bounds.y)};
  CGSize size{.width = static_cast<CGFloat>(bounds.width), .height = static_cast<CGFloat>(bounds.height)};

  AXValueRef positionValue = AXValueCreate(kAXValueTypeCGPoint, &position);
  AXValueRef sizeValue = AXValueCreate(kAXValueTypeCGSize, &size);
  if (!positionValue || !sizeValue) {
    if (positionValue) CFRelease(positionValue);
    if (sizeValue) CFRelease(sizeValue);
    return false;
  }

  bool posOk = AXUIElementSetAttributeValue(element, kAXPositionAttribute, positionValue) == kAXErrorSuccess;
  bool sizeOk = AXUIElementSetAttributeValue(element, kAXSizeAttribute, sizeValue) == kAXErrorSuccess;

  CFRelease(positionValue);
  CFRelease(sizeValue);

  if (posOk || sizeOk) scheduleRebuild();

  return posOk && sizeOk;
}

void MacosWindowManager::notifyWindowsChanged() { scheduleCoalescedRebuild(); }

void MacosWindowManager::notifyFocusChanged() { emit focusChanged(); }
