#include "macos-chrome-attached.hpp"

#include <QQuickItem>
#include <QQuickWindow>
#include <qevent.h>
#include <qlogging.h>

#import <AppKit/AppKit.h>
#import <objc/message.h>

namespace {

NSView *nsViewFromWinId(WId winId) { return (__bridge NSView *)reinterpret_cast<void *>(winId); }

NSVisualEffectMaterial materialFromString(const QString &name) {
  if (name == "popover") return NSVisualEffectMaterialPopover;
  if (name == "menu") return NSVisualEffectMaterialMenu;
  if (name == "sheet") return NSVisualEffectMaterialSheet;
  if (name == "sidebar") return NSVisualEffectMaterialSidebar;
  if (name == "windowBackground") return NSVisualEffectMaterialWindowBackground;
  if (name == "underWindowBackground") return NSVisualEffectMaterialUnderWindowBackground;
  if (name == "contentBackground") return NSVisualEffectMaterialContentBackground;
  return NSVisualEffectMaterialHUDWindow;
}

NSVisualEffectView *findEffectView(NSView *root) {
  for (NSView *sub in root.subviews) {
    if ([sub isKindOfClass:[NSVisualEffectView class]]) { return (NSVisualEffectView *)sub; }
  }
  return nil;
}

CGColorRef cgColorFromQColor(const QColor &c) {
  CGFloat comps[4] = {c.redF(), c.greenF(), c.blueF(), c.alphaF()};
  CGColorSpaceRef space = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
  CGColorRef cg = CGColorCreate(space, comps);
  CGColorSpaceRelease(space);
  return cg;
}

void installEffectView(NSWindow *nswin, bool enabled, NSVisualEffectMaterial material,
                       int cornerRadius, const QColor &borderColor, int borderWidth) {
  NSView *contentView = nswin.contentView;
  if (!contentView) return;

  // The blur must be a sibling of Qt's view, not a child: CALayer paints
  // parent then descendants, so a child blur would hide Qt's rendering.
  NSView *parent = contentView.superview;
  if (!parent) return;

  NSVisualEffectView *existing = findEffectView(parent);

  if (!enabled) {
    [existing removeFromSuperview];
    return;
  }

  if (!existing) {
    existing = [[NSVisualEffectView alloc] initWithFrame:parent.bounds];
    existing.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    existing.blendingMode = NSVisualEffectBlendingModeBehindWindow;
    existing.state = NSVisualEffectStateActive;
    existing.wantsLayer = YES;
    [parent addSubview:existing positioned:NSWindowBelow relativeTo:contentView];
  }
  existing.material = material;
  existing.layer.cornerRadius = cornerRadius;
  existing.layer.masksToBounds = YES;
  existing.layer.borderWidth = borderWidth;
  CGColorRef cg = cgColorFromQColor(borderColor);
  existing.layer.borderColor = cg;
  CGColorRelease(cg);
}

} // namespace

// ---------------- MacOSWindowAttached ----------------

MacOSWindowAttached::MacOSWindowAttached(QObject *parent) : QObject(parent) {
  m_window = qobject_cast<QWindow *>(parent);
  if (m_window) {
    trackWindow(m_window);
    return;
  }
  m_item = qobject_cast<QQuickItem *>(parent);
  if (m_item) {
    connect(m_item, &QQuickItem::windowChanged, this, &MacOSWindowAttached::onWindowChanged);
    if (m_item->window()) onWindowChanged(m_item->window());
  }
}

void MacOSWindowAttached::setEnabled(bool value) {
  if (m_enabled == value) return;
  m_enabled = value;
  emit enabledChanged();
  if (m_enabled) {
    apply();
  } else {
    revert();
  }
}

void MacOSWindowAttached::setCornerRadius(int value) {
  if (m_cornerRadius == value) return;
  m_cornerRadius = value;
  emit cornerRadiusChanged();
  apply();
}

void MacOSWindowAttached::setBlurEnabled(bool value) {
  if (m_blurEnabled == value) return;
  m_blurEnabled = value;
  emit blurEnabledChanged();
  apply();
}

void MacOSWindowAttached::setMaterial(const QString &value) {
  if (m_material == value) return;
  m_material = value;
  emit materialChanged();
  apply();
}

void MacOSWindowAttached::setBorderColor(const QColor &value) {
  if (m_borderColor == value) return;
  m_borderColor = value;
  emit borderColorChanged();
  apply();
}

void MacOSWindowAttached::setBorderWidth(int value) {
  if (m_borderWidth == value) return;
  m_borderWidth = value;
  emit borderWidthChanged();
  apply();
}

void MacOSWindowAttached::trackWindow(QWindow *window) {
  m_window = window;
  if (!m_window) return;
  m_window->installEventFilter(this);
}

void MacOSWindowAttached::onWindowChanged(QQuickWindow *window) {
  if (m_window) {
    m_window->removeEventFilter(this);
    m_window = nullptr;
    m_surfaceReady = false;
    m_snapshot = {};
  }
  if (window) {
    trackWindow(window);
    apply();
  }
}

void MacOSWindowAttached::apply() {
  if (!m_window || !m_enabled) return;
  if (!m_surfaceReady) {
    if (!m_window->handle()) return;
    m_surfaceReady = true;
  }

  NSView *nsview = nsViewFromWinId(m_window->winId());
  if (!nsview) return;
  NSWindow *nswin = nsview.window;
  if (!nswin) return;

  if (!m_snapshot.valid) {
    m_snapshot.valid = true;
    m_snapshot.opaque = nswin.opaque;
    m_snapshot.backgroundColor = (void *)CFBridgingRetain(nswin.backgroundColor);
    m_snapshot.hasShadow = nswin.hasShadow;
  }

  nswin.opaque = NO;
  nswin.backgroundColor = NSColor.clearColor;
  nswin.hasShadow = YES;

  installEffectView(nswin, m_blurEnabled, materialFromString(m_material), m_cornerRadius,
                    m_borderColor, m_borderWidth);
}

void MacOSWindowAttached::revert() {
  if (!m_window) return;
  NSView *nsview = nsViewFromWinId(m_window->winId());
  if (!nsview) return;
  NSWindow *nswin = nsview.window;
  if (!nswin) return;

  installEffectView(nswin, /*enabled=*/false, NSVisualEffectMaterialHUDWindow, 0, QColor(), 0);

  if (m_snapshot.valid) {
    nswin.opaque = m_snapshot.opaque;
    nswin.hasShadow = m_snapshot.hasShadow;
    if (m_snapshot.backgroundColor) {
      NSColor *bg = (NSColor *)CFBridgingRelease(m_snapshot.backgroundColor);
      m_snapshot.backgroundColor = nullptr;
      nswin.backgroundColor = bg;
    }
    m_snapshot = {};
  }
}

bool MacOSWindowAttached::eventFilter(QObject *obj, QEvent *event) {
  if (obj == m_window && event->type() == QEvent::PlatformSurface) {
    auto *se = static_cast<QPlatformSurfaceEvent *>(event); // NOLINT
    if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
      m_surfaceReady = true;
      apply();
    } else if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
      m_surfaceReady = false;
    }
  }
  return QObject::eventFilter(obj, event);
}

// ---------------- MacOSPanelAttached ----------------

MacOSPanelAttached::MacOSPanelAttached(QObject *parent) : QObject(parent) {
  m_window = qobject_cast<QWindow *>(parent);
  if (m_window) {
    trackWindow(m_window);
    return;
  }
  m_item = qobject_cast<QQuickItem *>(parent);
  if (m_item) {
    connect(m_item, &QQuickItem::windowChanged, this, &MacOSPanelAttached::onWindowChanged);
    if (m_item->window()) onWindowChanged(m_item->window());
  }
}

MacOSPanelAttached::~MacOSPanelAttached() { removeResignKeyObserver(); }

void MacOSPanelAttached::setEnabled(bool value) {
  if (m_enabled == value) return;
  m_enabled = value;
  emit enabledChanged();
  if (m_enabled) {
    apply();
  } else {
    revert();
    removeResignKeyObserver();
  }
}

void MacOSPanelAttached::setWindowLevel(int value) {
  if (m_windowLevel == value) return;
  m_windowLevel = value;
  emit windowLevelChanged();
  if (m_enabled) apply();
}

void MacOSPanelAttached::trackWindow(QWindow *window) {
  m_window = window;
  if (!m_window) return;
  m_window->installEventFilter(this);
}

void MacOSPanelAttached::onWindowChanged(QQuickWindow *window) {
  if (m_window) {
    m_window->removeEventFilter(this);
    m_window = nullptr;
    m_surfaceReady = false;
    m_snapshot = {};
    removeResignKeyObserver();
  }
  if (window) {
    trackWindow(window);
    apply();
  }
}

void MacOSPanelAttached::installResignKeyObserver(void *nswinPtr) {
  NSWindow *nswin = (__bridge NSWindow *)nswinPtr;
  if (m_observedNSWindow == nswinPtr && m_resignKeyObserver) return;

  removeResignKeyObserver();

  id token = [[NSNotificationCenter defaultCenter]
      addObserverForName:NSWindowDidResignKeyNotification
                  object:nswin
                   queue:[NSOperationQueue mainQueue]
              usingBlock:^(NSNotification *) { emit this->resignKey(); }];

  m_resignKeyObserver = (void *)CFBridgingRetain(token);
  m_observedNSWindow = nswinPtr;
}

void MacOSPanelAttached::removeResignKeyObserver() {
  if (!m_resignKeyObserver) return;
  id token = CFBridgingRelease(m_resignKeyObserver);
  [[NSNotificationCenter defaultCenter] removeObserver:token];
  m_resignKeyObserver = nullptr;
  m_observedNSWindow = nullptr;
}

void MacOSPanelAttached::apply() {
  if (!m_window || !m_enabled) return;
  if (!m_surfaceReady) {
    if (!m_window->handle()) return;
    m_surfaceReady = true;
  }

  NSView *nsview = nsViewFromWinId(m_window->winId());
  if (!nsview) return;
  NSWindow *nswin = nsview.window;
  if (!nswin) return;

  if (!m_snapshot.valid) {
    m_snapshot.valid = true;
    m_snapshot.styleMask = (unsigned long)nswin.styleMask;
    m_snapshot.level = (int)nswin.level;
    m_snapshot.collectionBehavior = (unsigned long)nswin.collectionBehavior;
    m_snapshot.hidesOnDeactivate = nswin.hidesOnDeactivate;
    m_snapshot.movableByWindowBackground = nswin.movableByWindowBackground;
    m_snapshot.isPanel = [nswin isKindOfClass:[NSPanel class]];
    if (m_snapshot.isPanel) {
      NSPanel *panel = (NSPanel *)nswin;
      m_snapshot.floatingPanel = panel.floatingPanel;
      m_snapshot.becomesKeyOnlyIfNeeded = panel.becomesKeyOnlyIfNeeded;
      m_snapshot.worksWhenModal = panel.worksWhenModal;
    }
  }

  // Make this a non-activating panel so the launcher can take key focus
  // without making vicinae the foreground app — same model Spotlight/Raycast use.
  NSWindowStyleMask mask = nswin.styleMask;
  if (!(mask & NSWindowStyleMaskNonactivatingPanel)) {
    mask |= NSWindowStyleMaskNonactivatingPanel;
    [nswin setStyleMask:mask];
  }

  // AppKit caches the WindowServer "prevents activation" tag at -[NSWindow init];
  // setStyleMask alone doesn't refresh it. Force-sync via the private SPI.
  SEL preventsSel = NSSelectorFromString(@"_setPreventsActivation:");
  if ([nswin respondsToSelector:preventsSel]) {
    ((void (*)(id, SEL, BOOL))objc_msgSend)(nswin, preventsSel, YES);
  } else {
    qWarning() << "macos-chrome: -[NSWindow _setPreventsActivation:] is unavailable; panel will steal focus";
  }

  nswin.level = m_windowLevel;
  nswin.collectionBehavior = NSWindowCollectionBehaviorCanJoinAllSpaces |
                             NSWindowCollectionBehaviorFullScreenAuxiliary |
                             NSWindowCollectionBehaviorTransient |
                             NSWindowCollectionBehaviorIgnoresCycle;
  nswin.hidesOnDeactivate = NO;
  nswin.movableByWindowBackground = NO;

  if ([nswin isKindOfClass:[NSPanel class]]) {
    NSPanel *panel = (NSPanel *)nswin;
    panel.floatingPanel = YES;
    panel.becomesKeyOnlyIfNeeded = NO;
    panel.worksWhenModal = YES;
  }

  installResignKeyObserver((__bridge void *)nswin);
}

void MacOSPanelAttached::revert() {
  if (!m_window || !m_snapshot.valid) return;
  NSView *nsview = nsViewFromWinId(m_window->winId());
  if (!nsview) return;
  NSWindow *nswin = nsview.window;
  if (!nswin) return;

  [nswin setStyleMask:(NSWindowStyleMask)m_snapshot.styleMask];

  SEL preventsSel = NSSelectorFromString(@"_setPreventsActivation:");
  if ([nswin respondsToSelector:preventsSel]) {
    BOOL prevents = (m_snapshot.styleMask & NSWindowStyleMaskNonactivatingPanel) ? YES : NO;
    ((void (*)(id, SEL, BOOL))objc_msgSend)(nswin, preventsSel, prevents);
  }

  nswin.level = m_snapshot.level;
  nswin.collectionBehavior = (NSWindowCollectionBehavior)m_snapshot.collectionBehavior;
  nswin.hidesOnDeactivate = m_snapshot.hidesOnDeactivate;
  nswin.movableByWindowBackground = m_snapshot.movableByWindowBackground;

  if (m_snapshot.isPanel && [nswin isKindOfClass:[NSPanel class]]) {
    NSPanel *panel = (NSPanel *)nswin;
    panel.floatingPanel = m_snapshot.floatingPanel;
    panel.becomesKeyOnlyIfNeeded = m_snapshot.becomesKeyOnlyIfNeeded;
    panel.worksWhenModal = m_snapshot.worksWhenModal;
  }

  m_snapshot = {};
}

bool MacOSPanelAttached::eventFilter(QObject *obj, QEvent *event) {
  if (obj == m_window && event->type() == QEvent::PlatformSurface) {
    auto *se = static_cast<QPlatformSurfaceEvent *>(event); // NOLINT
    if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
      m_surfaceReady = true;
      apply();
    } else if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
      m_surfaceReady = false;
      removeResignKeyObserver();
    }
  }
  return QObject::eventFilter(obj, event);
}

// ---------------- App-level helpers ----------------

void macosSetAccessoryActivationPolicy() {
  [NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];
}

void macosActivateApp() { [NSApp activateIgnoringOtherApps:YES]; }
