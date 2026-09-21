#include "macos-window-manager.hpp"

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#include <IOKit/graphics/IOGraphicsTypes.h>

#include <unistd.h>
#include <QFutureWatcher>
#include <QPointer>
#include <QTimer>
#include <QtConcurrent>
#include <array>
#include <chrono>
#include <cstring>
#include <unordered_set>

#include "macos-window.hpp"
#include "macos-spaces.hpp"

// Private but stable HIServices APIs used by virtually every macOS window switcher (AltTab,
// HyperSwitch, ...). _AXUIElementGetWindow maps an AX window element to its CoreGraphics window id,
// giving us a stable identity. _AXUIElementCreateWithRemoteToken builds an AX element straight from
// a (pid, element id) token, which lets us reach windows living on other Spaces:
// kAXWindowsAttribute only ever returns windows of the current Space. Both degrade gracefully
// (failed calls are simply skipped) if Apple ever changes them.
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

bool axIsSettable(AXUIElementRef element, CFStringRef attribute) {
  Boolean settable = false;
  return AXUIElementIsAttributeSettable(element, attribute, &settable) == kAXErrorSuccess && settable;
}

CFStringRef const kAXFullScreenAttributeName = CFSTR("AXFullScreen");

bool isWindowLike(AXUIElementRef element) {
  QString subrole = axCopyString(element, kAXSubroleAttribute);
  if (subrole.isEmpty()) return true;
  return subrole == QString::fromCFString(kAXStandardWindowSubrole) ||
         subrole == QString::fromCFString(kAXDialogSubrole);
}

// Stricter than isWindowLike: brute-forced element ids resolve to all kinds of UI elements
// (buttons, menus,
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
  bool canFullScreen = axIsSettable(element, kAXFullScreenAttributeName);

  return std::make_shared<MacosWindow>(element, std::move(id), std::move(title), bundleId, pid, canClose,
                                       canFullScreen);
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

// Windows on the current Space, as reported by the AX windows attribute. Elements are owned by the
// returned array, so callers must not release them individually (MacosWindow retains the ones it
// keeps).
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

// Must run on the main thread: NSWorkspace is not safe to query off-main. We only collect
// lightweight descriptors here so the expensive AX walk can happen on a worker thread.
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

// The full AX scan is expensive (the per-app brute-force needed for other-Space windows is
// cross-process and budgeted), so it runs on a worker thread and the result is swapped into the
// cache on the main thread. Only one scan runs at a time; requests arriving mid-scan are coalesced
// into a single follow-up.
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

static std::optional<QSize> displayScanoutSize(CGDirectDisplayID display) {
  CGDisplayModeRef current = CGDisplayCopyDisplayMode(display);
  if (!current) return std::nullopt;

  const QSize pixels(CGDisplayModeGetPixelWidth(current), CGDisplayModeGetPixelHeight(current));
  const bool scaled = CGDisplayModeGetPixelWidth(current) != CGDisplayModeGetWidth(current);
  CGDisplayModeRelease(current);

  if (!scaled) return pixels;

  CFArrayRef modes = CGDisplayCopyAllDisplayModes(display, nullptr);
  if (!modes) return pixels;

  std::optional<QSize> native;

  for (CFIndex i = 0; i < CFArrayGetCount(modes); ++i) {
    auto mode = (CGDisplayModeRef)CFArrayGetValueAtIndex(modes, i);

    if (CGDisplayModeGetIOFlags(mode) & kDisplayModeNativeFlag) {
      native = QSize(CGDisplayModeGetPixelWidth(mode), CGDisplayModeGetPixelHeight(mode));
      break;
    }
  }

  CFRelease(modes);
  return native ? *native : pixels;
}

static CGDirectDisplayID screenDisplayId(NSScreen *screen) {
  return (CGDirectDisplayID)[screen.deviceDescription[@"NSScreenNumber"] unsignedIntValue];
}

static std::optional<CGDirectDisplayID> windowDisplayId(QWindow *window) {
  if (!window || !window->isVisible() || !window->handle()) return std::nullopt;

  auto *view = (__bridge NSView *)reinterpret_cast<void *>(window->winId());
  NSScreen *screen = view.window.screen;
  if (!screen) return std::nullopt;

  return screenDisplayId(screen);
}

std::vector<AbstractWindowManager::Screen> MacosWindowManager::listScreensSync(QWindow *activeWindow) const {
  std::vector<Screen> screens;

  @autoreleasepool {
    auto activeDisplay = windowDisplayId(activeWindow);
    NSArray<NSScreen *> *nsScreens = [NSScreen screens];
    screens.reserve(nsScreens.count);

    for (NSScreen *nsScreen in nsScreens) {
      auto display = screenDisplayId(nsScreen);
      const CGRect bounds = CGDisplayBounds(display);
      const QSize logicalSize(qRound(bounds.size.width), qRound(bounds.size.height));

      const NSRect frame = nsScreen.frame;
      const NSRect visible = nsScreen.visibleFrame;
      const QRect available(qRound(bounds.origin.x + NSMinX(visible) - NSMinX(frame)),
                            qRound(bounds.origin.y + NSMaxY(frame) - NSMaxY(visible)),
                            qRound(visible.size.width), qRound(visible.size.height));

      Screen screen{.name = QString::fromNSString(nsScreen.localizedName),
                    .bounds = QRect(QPoint(qRound(bounds.origin.x), qRound(bounds.origin.y)), logicalSize),
                    .availableBounds = available,
                    .physicalResolution =
                        displayScanoutSize(display).value_or(logicalSize * nsScreen.backingScaleFactor)};
      screen.active = activeDisplay == display;
      screens.emplace_back(std::move(screen));
    }
  }

  return screens;
}

AbstractWindowManager::WindowPtr MacosWindowManager::getFocusedWindowSync() const {
  if (!AXIsProcessTrusted()) return nullptr;

  WindowPtr result;

  @autoreleasepool {
    NSRunningApplication *front = [[NSWorkspace sharedWorkspace] frontmostApplication];
    if (!front) return nullptr;

    pid_t pid = front.processIdentifier;
    if (pid <= 0 || pid == getpid()) return nullptr;

    QString bundleId = front.bundleIdentifier ? QString::fromNSString(front.bundleIdentifier) : QString();
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
      NSRunningApplication *app = [NSRunningApplication runningApplicationWithProcessIdentifier:*pid];
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

QFlags<AbstractWindowManager::Capability> MacosWindowManager::capabilities() const {
  QFlags<Capability> result{Capability::WindowPlacement, Capability::Minimize};
  if (MacosSpaces::supportsWindowMove()) result.setFlag(Capability::MoveToAdjacentWorkspace);
  if (MacosSpaces::supportsSpaceSwitch()) result.setFlag(Capability::SwitchToAdjacentWorkspace);
  return result;
}

bool MacosWindowManager::minimizeWindow(const AbstractWindow &window) const {
  const auto *macWindow = asMacosWindow(window);
  if (!macWindow || macWindow->fullScreen() || !axIsSettable(macWindow->element(), kAXMinimizedAttribute))
    return false;
  const bool ok = AXUIElementSetAttributeValue(macWindow->element(), kAXMinimizedAttribute, kCFBooleanTrue) ==
                  kAXErrorSuccess;
  if (ok) scheduleRebuild();
  return ok;
}

QFuture<AbstractWindowManager::WorkspaceChangeResult>
MacosWindowManager::moveToAdjacentWorkspace(const AbstractWindow &window, Direction direction) {
  if (m_changingWorkspace) return QtFuture::makeReadyValueFuture(WorkspaceChangeResult::Busy);
  const auto *macWindow = asMacosWindow(window);
  if (!macWindow || macWindow->fullScreen())
    return QtFuture::makeReadyValueFuture(WorkspaceChangeResult::Unsupported);
  m_changingWorkspace = true;
  return MacosSpaces::moveWindow(macWindow->element(), direction, this)
      .then(this, [this](WorkspaceChangeResult result) {
        m_changingWorkspace = false;
        if (result == WorkspaceChangeResult::Success || result == WorkspaceChangeResult::FollowFailed)
          scheduleRebuild();
        return result;
      });
}

QFuture<AbstractWindowManager::WorkspaceChangeResult>
MacosWindowManager::switchToAdjacentWorkspace(Direction direction) {
  if (m_changingWorkspace) return QtFuture::makeReadyValueFuture(WorkspaceChangeResult::Busy);
  m_changingWorkspace = true;
  return MacosSpaces::switchSpace(direction, this).then(this, [this](WorkspaceChangeResult result) {
    m_changingWorkspace = false;
    return result;
  });
}

bool MacosWindowManager::setWindowBounds(const AbstractWindow &window, const WindowBounds &bounds) const {
  const MacosWindow *macWindow = asMacosWindow(window);
  if (!macWindow || bounds.width <= 0 || bounds.height <= 0 || macWindow->fullScreen()) return false;

  AXUIElementRef element = macWindow->element();
  const auto current = macWindow->bounds();
  if (!current) return false;
  const bool resize = current->width != bounds.width || current->height != bounds.height;
  if (!axIsSettable(element, kAXPositionAttribute) || (resize && !axIsSettable(element, kAXSizeAttribute)))
    return false;

  CGPoint position{.x = static_cast<CGFloat>(bounds.x), .y = static_cast<CGFloat>(bounds.y)};
  CGSize size{.width = static_cast<CGFloat>(bounds.width), .height = static_cast<CGFloat>(bounds.height)};

  AXValueRef positionValue = AXValueCreate(kAXValueTypeCGPoint, &position);
  AXValueRef sizeValue = AXValueCreate(kAXValueTypeCGSize, &size);
  if (!positionValue || !sizeValue) {
    if (positionValue) CFRelease(positionValue);
    if (sizeValue) CFRelease(sizeValue);
    return false;
  }

  // Shrink before moving toward an edge, then retry the size after moving to another display.
  bool sizeOk =
      !resize || AXUIElementSetAttributeValue(element, kAXSizeAttribute, sizeValue) == kAXErrorSuccess;
  const bool posOk =
      AXUIElementSetAttributeValue(element, kAXPositionAttribute, positionValue) == kAXErrorSuccess;
  if (resize) sizeOk = AXUIElementSetAttributeValue(element, kAXSizeAttribute, sizeValue) == kAXErrorSuccess;

  CFRelease(positionValue);
  CFRelease(sizeValue);

  if (posOk || sizeOk) scheduleRebuild();

  return posOk && sizeOk;
}

void MacosWindowManager::notifyWindowsChanged() { scheduleCoalescedRebuild(); }

void MacosWindowManager::notifyFocusChanged() { emit focusChanged(); }
