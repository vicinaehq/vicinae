#include "macos-selection-service.hpp"
#import <AppKit/AppKit.h>
#include <ApplicationServices/ApplicationServices.h>
#include <QPromise>
#include <QTimer>
#include <QtLogging>
#include <unistd.h>

namespace {

using Result = AbstractSelectionService::Result;

constexpr CGKeyCode VK_ANSI_C = 0x08;
constexpr int COPY_POLL_INTERVAL_MS = 10;
constexpr int COPY_TIMEOUT_MS = 400;
NSString *const TRANSIENT_UTI = @"org.nspasteboard.TransientType";

QFuture<Result> ready(Result result) { return QtFuture::makeReadyValueFuture<Result>(std::move(result)); }

QFuture<Result> fail(const QString &error) { return ready(std::unexpected(error)); }

std::optional<pid_t> frontmostPid() {
  NSRunningApplication *front = [[NSWorkspace sharedWorkspace] frontmostApplication];
  if (!front) return std::nullopt;

  pid_t pid = front.processIdentifier;
  if (pid <= 0 || pid == getpid()) return std::nullopt;

  return pid;
}

QString accessibilitySelectedText(pid_t pid) {
  QString result;
  AXUIElementRef app = AXUIElementCreateApplication(pid);
  CFTypeRef focused = nullptr;

  if (AXUIElementCopyAttributeValue(app, kAXFocusedUIElementAttribute, &focused) == kAXErrorSuccess &&
      focused) {
    CFTypeRef value = nullptr;
    if (AXUIElementCopyAttributeValue(static_cast<AXUIElementRef>(focused), kAXSelectedTextAttribute,
                                      &value) == kAXErrorSuccess &&
        value) {
      if (CFGetTypeID(value) == CFStringGetTypeID()) {
        result = QString::fromCFString(static_cast<CFStringRef>(value));
      }
      CFRelease(value);
    }
    CFRelease(focused);
  }

  CFRelease(app);
  return result;
}

void postKeyToPid(pid_t pid, CGKeyCode key, bool down, CGEventFlags flags) {
  CGEventRef event = CGEventCreateKeyboardEvent(nullptr, key, down);
  if (!event) return;
  CGEventSetFlags(event, flags);
  CGEventPostToPid(pid, event);
  CFRelease(event);
}

NSArray<NSPasteboardItem *> *snapshotPasteboard(NSPasteboard *pb) {
  NSMutableArray<NSPasteboardItem *> *copies = [NSMutableArray array];

  for (NSPasteboardItem *item in pb.pasteboardItems) {
    NSPasteboardItem *copy = [[NSPasteboardItem alloc] init];
    for (NSPasteboardType type in item.types) {
      if (NSData *data = [item dataForType:type]) { [copy setData:data forType:type]; }
    }
    if (copy.types.count > 0) { [copies addObject:copy]; }
  }

  return copies;
}

// The restore is flagged transient so the clipboard monitor doesn't re-index what was already there.
void restorePasteboard(NSPasteboard *pb, NSArray<NSPasteboardItem *> *items) {
  [pb clearContents];
  if (items.count == 0) return;
  [items.firstObject setString:@"" forType:TRANSIENT_UTI];
  [pb writeObjects:items];
}

} // namespace

QFuture<Result> MacosSelectionService::selectedText() {
  if (!AXIsProcessTrusted()) {
    return fail(QStringLiteral("Accessibility permission is required to read the selected text"));
  }

  @autoreleasepool {
    auto pid = frontmostPid();
    if (!pid) return fail(QStringLiteral("Unable to get selected text"));

    if (QString text = accessibilitySelectedText(*pid); !text.isEmpty()) { return ready(std::move(text)); }

    return copyFromFrontmostApp(*pid);
  }
}

QFuture<Result> MacosSelectionService::copyFromFrontmostApp(int pid) {
  NSPasteboard *pb = [NSPasteboard generalPasteboard];
  NSInteger const before = pb.changeCount;
  NSArray<NSPasteboardItem *> *snapshot = snapshotPasteboard(pb);

  postKeyToPid(pid, VK_ANSI_C, true, kCGEventFlagMaskCommand);
  postKeyToPid(pid, VK_ANSI_C, false, kCGEventFlagMaskCommand);

  auto promise = std::make_shared<QPromise<Result>>();
  promise->start();

  auto *timer = new QTimer(this);
  auto elapsed = std::make_shared<int>(0);

  timer->setInterval(COPY_POLL_INTERVAL_MS);
  connect(timer, &QTimer::timeout, this, [timer, promise, elapsed, before, snapshot]() {
    *elapsed += COPY_POLL_INTERVAL_MS;

    @autoreleasepool {
      NSPasteboard *pb = [NSPasteboard generalPasteboard];

      if (pb.changeCount == before) {
        if (*elapsed < COPY_TIMEOUT_MS) return;
        timer->deleteLater();
        promise->addResult(std::unexpected(QStringLiteral("Unable to get selected text")));
        promise->finish();
        return;
      }

      timer->deleteLater();
      NSString *str = [pb stringForType:NSPasteboardTypeString];
      QString text = str ? QString::fromNSString(str) : QString();
      restorePasteboard(pb, snapshot);

      if (text.isEmpty()) {
        promise->addResult(std::unexpected(QStringLiteral("Unable to get selected text")));
      } else {
        promise->addResult(std::move(text));
      }
      promise->finish();
    }
  });
  timer->start();

  return promise->future();
}
