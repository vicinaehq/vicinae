#include "macos-menu-bar.hpp"

#import <AppKit/AppKit.h>
#import <ApplicationServices/ApplicationServices.h>
#import <Carbon/Carbon.h>

#include <QtConcurrent/QtConcurrent>
#include <unistd.h>

namespace {

constexpr int MAX_DEPTH = 6;
constexpr size_t MAX_ENTRIES = 3000;
constexpr float AX_TIMEOUT_SECONDS = 0.5f;
constexpr useconds_t REVEAL_STEP_DELAY_US = 120 * 1000;

std::shared_ptr<const void> retained(CFTypeRef ref) {
  CFRetain(ref);
  return {static_cast<const void *>(ref), [](const void *p) { CFRelease(p); }};
}

AXUIElementRef axRef(const std::shared_ptr<const void> &p) {
  return static_cast<AXUIElementRef>(const_cast<void *>(p.get()));
}

QString stringValue(CFTypeRef value) {
  if (!value || CFGetTypeID(value) != CFStringGetTypeID()) return {};
  return QString::fromNSString((__bridge NSString *)value);
}

int intValue(CFTypeRef value, int fallback) {
  if (!value || CFGetTypeID(value) != CFNumberGetTypeID()) return fallback;
  int out = fallback;
  CFNumberGetValue(static_cast<CFNumberRef>(const_cast<void *>(value)), kCFNumberIntType, &out);
  return out;
}

bool boolValue(CFTypeRef value, bool fallback) {
  if (!value || CFGetTypeID(value) != CFBooleanGetTypeID()) return fallback;
  return CFBooleanGetValue(static_cast<CFBooleanRef>(const_cast<void *>(value)));
}

CFArrayRef arrayValue(CFTypeRef value) {
  if (!value || CFGetTypeID(value) != CFArrayGetTypeID()) return nullptr;
  return static_cast<CFArrayRef>(const_cast<void *>(value));
}

Qt::Key keyFromVirtualKey(int vk) {
  switch (vk) {
  case kVK_Return:
    return Qt::Key_Return;
  case kVK_ANSI_KeypadEnter:
    return Qt::Key_Enter;
  case kVK_Tab:
    return Qt::Key_Tab;
  case kVK_Space:
    return Qt::Key_Space;
  case kVK_Delete:
    return Qt::Key_Backspace;
  case kVK_ForwardDelete:
    return Qt::Key_Delete;
  case kVK_Escape:
    return Qt::Key_Escape;
  case kVK_Home:
    return Qt::Key_Home;
  case kVK_End:
    return Qt::Key_End;
  case kVK_PageUp:
    return Qt::Key_PageUp;
  case kVK_PageDown:
    return Qt::Key_PageDown;
  case kVK_LeftArrow:
    return Qt::Key_Left;
  case kVK_RightArrow:
    return Qt::Key_Right;
  case kVK_UpArrow:
    return Qt::Key_Up;
  case kVK_DownArrow:
    return Qt::Key_Down;
  case kVK_F1:
    return Qt::Key_F1;
  case kVK_F2:
    return Qt::Key_F2;
  case kVK_F3:
    return Qt::Key_F3;
  case kVK_F4:
    return Qt::Key_F4;
  case kVK_F5:
    return Qt::Key_F5;
  case kVK_F6:
    return Qt::Key_F6;
  case kVK_F7:
    return Qt::Key_F7;
  case kVK_F8:
    return Qt::Key_F8;
  case kVK_F9:
    return Qt::Key_F9;
  case kVK_F10:
    return Qt::Key_F10;
  case kVK_F11:
    return Qt::Key_F11;
  case kVK_F12:
    return Qt::Key_F12;
  case kVK_F13:
    return Qt::Key_F13;
  case kVK_F14:
    return Qt::Key_F14;
  case kVK_F15:
    return Qt::Key_F15;
  case kVK_F16:
    return Qt::Key_F16;
  case kVK_F17:
    return Qt::Key_F17;
  case kVK_F18:
    return Qt::Key_F18;
  case kVK_F19:
    return Qt::Key_F19;
  case kVK_F20:
    return Qt::Key_F20;
  default:
    return Qt::Key_unknown;
  }
}

// kAXMenuItemCmdModifiers bitmask: bit 0 = shift, bit 1 = option, bit 2 = control, bit 3 = command absent
Keyboard::Shortcut shortcutFor(const QString &cmdChar, int modMask, int vk) {
  Qt::Key key = Qt::Key_unknown;

  if (!cmdChar.isEmpty() && cmdChar.front().isPrint() && !cmdChar.front().isSpace()) {
    key = static_cast<Qt::Key>(cmdChar.front().toUpper().unicode());
  } else if (vk > 0) {
    key = keyFromVirtualKey(vk);
  }
  if (key == Qt::Key_unknown) return {};

  Qt::KeyboardModifiers mods;
  // Qt swaps Ctrl/Meta on macOS: ControlModifier == Cmd, MetaModifier == Ctrl
  if (!(modMask & 8)) mods |= Qt::ControlModifier;
  if (modMask & 1) mods |= Qt::ShiftModifier;
  if (modMask & 2) mods |= Qt::AltModifier;
  if (modMask & 4) mods |= Qt::MetaModifier;

  return {key, mods};
}

CFArrayRef copyItemAttributes(AXUIElementRef element) {
  static NSArray *names = @[
    (__bridge id)kAXTitleAttribute,
    (__bridge id)kAXEnabledAttribute,
    (__bridge id)kAXChildrenAttribute,
    (__bridge id)kAXMenuItemCmdCharAttribute,
    (__bridge id)kAXMenuItemCmdModifiersAttribute,
    (__bridge id)kAXMenuItemCmdVirtualKeyAttribute,
  ];

  AXUIElementSetMessagingTimeout(element, AX_TIMEOUT_SECONDS);
  CFArrayRef values = nullptr;
  if (AXUIElementCopyMultipleAttributeValues(element, (__bridge CFArrayRef)names, 0, &values) !=
      kAXErrorSuccess)
    return nullptr;
  return values;
}

// Walks a container whose children are menu items: the menu bar itself, or any AXMenu
void walkMenu(AXUIElementRef menu, int depth, const QStringList &path,
              const std::vector<std::shared_ptr<const void>> &ancestors, int pid,
              std::vector<MenuBar::Entry> &out) {
  if (depth > MAX_DEPTH || out.size() >= MAX_ENTRIES) return;

  AXUIElementSetMessagingTimeout(menu, AX_TIMEOUT_SECONDS);
  CFTypeRef childrenValue = nullptr;
  if (AXUIElementCopyAttributeValue(menu, kAXChildrenAttribute, &childrenValue) != kAXErrorSuccess) return;
  CFArrayRef children = arrayValue(childrenValue);
  if (!children) {
    if (childrenValue) CFRelease(childrenValue);
    return;
  }

  // The first menu bar item is always the Apple menu, which is the same for every app
  CFIndex start = depth == 0 ? 1 : 0;

  for (CFIndex i = start; i < CFArrayGetCount(children) && out.size() < MAX_ENTRIES; ++i) {
    auto item = static_cast<AXUIElementRef>(const_cast<void *>(CFArrayGetValueAtIndex(children, i)));
    CFArrayRef values = copyItemAttributes(item);
    if (!values || CFArrayGetCount(values) < 6) {
      if (values) CFRelease(values);
      continue;
    }

    QString title = stringValue(CFArrayGetValueAtIndex(values, 0)).trimmed();
    bool enabled = boolValue(CFArrayGetValueAtIndex(values, 1), true);
    CFArrayRef kids = arrayValue(CFArrayGetValueAtIndex(values, 2));
    QString cmdChar = stringValue(CFArrayGetValueAtIndex(values, 3));
    int cmdMods = intValue(CFArrayGetValueAtIndex(values, 4), 0);
    int cmdVk = intValue(CFArrayGetValueAtIndex(values, 5), 0);

    AXUIElementRef submenu = nullptr;
    if (kids && CFArrayGetCount(kids) > 0)
      submenu = static_cast<AXUIElementRef>(const_cast<void *>(CFArrayGetValueAtIndex(kids, 0)));

    if (title.isEmpty() || !enabled) {
      CFRelease(values);
      continue;
    }

    if (submenu) {
      auto nextAncestors = ancestors;
      nextAncestors.push_back(retained(item));
      QStringList nextPath = path;
      nextPath << title;
      walkMenu(submenu, depth + 1, nextPath, nextAncestors, pid, out);
    } else {
      MenuBar::Entry entry;
      entry.title = title;
      entry.path = path;
      entry.shortcut = shortcutFor(cmdChar, cmdMods, cmdVk);
      entry.pid = pid;
      entry.itemRef = retained(item);
      entry.ancestorRefs = ancestors;
      out.push_back(std::move(entry));
    }

    CFRelease(values);
  }

  CFRelease(children);
}

MenuBar::Snapshot scanSync(const MenuBar::Target &target) {
  MenuBar::Snapshot snapshot;
  snapshot.pid = target.pid;
  snapshot.appName = target.appName;
  snapshot.bundleId = target.bundleId;

  if (!target.pid || !AXIsProcessTrusted()) return snapshot;

  AXUIElementRef app = AXUIElementCreateApplication(target.pid);
  if (!app) return snapshot;

  AXUIElementSetMessagingTimeout(app, 1.0);
  CFTypeRef menuBar = nullptr;
  if (AXUIElementCopyAttributeValue(app, kAXMenuBarAttribute, &menuBar) == kAXErrorSuccess && menuBar) {
    walkMenu(static_cast<AXUIElementRef>(const_cast<void *>(menuBar)), 0, {}, {}, snapshot.pid,
             snapshot.entries);
    CFRelease(menuBar);
  }
  CFRelease(app);

  return snapshot;
}

// Re-resolves an entry by walking titles again, for when the app rebuilt its menus
// and the retained element reference went stale
std::shared_ptr<const void> resolveByPath(const MenuBar::Entry &entry) {
  AXUIElementRef app = AXUIElementCreateApplication(entry.pid);
  if (!app) return nullptr;

  AXUIElementSetMessagingTimeout(app, 1.0);
  CFTypeRef container = nullptr;
  AXError err = AXUIElementCopyAttributeValue(app, kAXMenuBarAttribute, &container);
  CFRelease(app);
  if (err != kAXErrorSuccess || !container) return nullptr;

  QStringList segments = entry.path;
  segments << entry.title;

  for (qsizetype s = 0; s < segments.size(); ++s) {
    auto containerEl = static_cast<AXUIElementRef>(const_cast<void *>(container));
    AXUIElementSetMessagingTimeout(containerEl, AX_TIMEOUT_SECONDS);

    CFTypeRef childrenValue = nullptr;
    AXUIElementCopyAttributeValue(containerEl, kAXChildrenAttribute, &childrenValue);
    CFRelease(container);
    container = nullptr;

    CFArrayRef children = arrayValue(childrenValue);
    if (!children) {
      if (childrenValue) CFRelease(childrenValue);
      return nullptr;
    }

    CFTypeRef match = nullptr;
    for (CFIndex i = 0; i < CFArrayGetCount(children); ++i) {
      auto child = static_cast<AXUIElementRef>(const_cast<void *>(CFArrayGetValueAtIndex(children, i)));
      CFTypeRef titleValue = nullptr;
      AXUIElementCopyAttributeValue(child, kAXTitleAttribute, &titleValue);
      bool found = stringValue(titleValue).trimmed() == segments[s];
      if (titleValue) CFRelease(titleValue);
      if (found) {
        match = child;
        CFRetain(match);
        break;
      }
    }
    CFRelease(children);
    if (!match) return nullptr;

    if (s + 1 == segments.size()) {
      auto result = retained(match);
      CFRelease(match);
      return result;
    }

    // Descend through the matched item's AXMenu wrapper
    CFTypeRef kidsValue = nullptr;
    AXUIElementCopyAttributeValue(static_cast<AXUIElementRef>(const_cast<void *>(match)),
                                  kAXChildrenAttribute, &kidsValue);
    CFRelease(match);
    CFArrayRef kids = arrayValue(kidsValue);
    if (!kids || CFArrayGetCount(kids) == 0) {
      if (kidsValue) CFRelease(kidsValue);
      return nullptr;
    }
    container = CFArrayGetValueAtIndex(kids, 0);
    CFRetain(container);
    CFRelease(kids);
  }

  if (container) CFRelease(container);
  return nullptr;
}

} // namespace

namespace MenuBar {

bool isSupported() { return AXIsProcessTrusted(); }

Target frontmostTarget() {
  @autoreleasepool {
    NSRunningApplication *front = [[NSWorkspace sharedWorkspace] frontmostApplication];
    if (!front || front.processIdentifier == getpid()) return {};
    return {
        .pid = front.processIdentifier,
        .appName = QString::fromNSString(front.localizedName ?: @""),
        .bundleId = QString::fromNSString(front.bundleIdentifier ?: @""),
    };
  }
}

QFuture<Snapshot> scan(Target target) {
  return QtConcurrent::run([target = std::move(target)] { return scanSync(target); });
}

void pressItem(Entry entry) {
  auto future = QtConcurrent::run([entry = std::move(entry)] {
    AXUIElementSetMessagingTimeout(axRef(entry.itemRef), 1.0);
    if (AXUIElementPerformAction(axRef(entry.itemRef), kAXPressAction) == kAXErrorSuccess) return;
    if (auto fresh = resolveByPath(entry)) AXUIElementPerformAction(axRef(fresh), kAXPressAction);
  });
  Q_UNUSED(future);
}

void revealInMenuBar(Entry entry) {
  auto future = QtConcurrent::run([entry = std::move(entry)] {
    usleep(REVEAL_STEP_DELAY_US);
    for (const auto &ancestor : entry.ancestorRefs) {
      AXUIElementSetMessagingTimeout(axRef(ancestor), 1.0);
      if (AXUIElementPerformAction(axRef(ancestor), kAXPressAction) != kAXErrorSuccess) break;
      usleep(REVEAL_STEP_DELAY_US);
    }
  });
  Q_UNUSED(future);
}

} // namespace MenuBar
