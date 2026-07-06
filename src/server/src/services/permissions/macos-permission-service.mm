#include "macos-permission-service.hpp"
#include <ApplicationServices/ApplicationServices.h>
#include <fcntl.h>
#include <unistd.h>
#import <AppKit/AppKit.h>

namespace {

constexpr int POLL_INTERVAL_MS = 1000;

constexpr const char *ACCESSIBILITY_PANE_URL =
    "x-apple.systempreferences:com.apple.preference.security?Privacy_Accessibility";

constexpr const char *FULL_DISK_PANE_URL =
    "x-apple.systempreferences:com.apple.preference.security?Privacy_AllFiles";

// FDA has no query API; readability of a TCC-protected file is the standard probe
bool probeFullDiskAccess() {
  NSArray<NSString *> *probes = @[
    [@"~/Library/Application Support/com.apple.TCC/TCC.db" stringByExpandingTildeInPath],
    @"/Library/Application Support/com.apple.TCC/TCC.db",
  ];

  for (NSString *path in probes) {
    int fd = open(path.fileSystemRepresentation, O_RDONLY);
    if (fd >= 0) {
      close(fd);
      return true;
    }
  }

  return false;
}

void openPane(const char *url) {
  [NSWorkspace.sharedWorkspace openURL:[NSURL URLWithString:@(url)]];
}

} // namespace

MacosPermissionService::MacosPermissionService(QObject *parent) : QObject(parent) {
  m_accessibilityGranted = AXIsProcessTrusted();
  m_fullDiskAccessGranted = probeFullDiskAccess();
  m_pollTimer.setInterval(POLL_INTERVAL_MS);
  connect(&m_pollTimer, &QTimer::timeout, this, &MacosPermissionService::refresh);
}

void MacosPermissionService::setWatching(bool value) {
  if (value == m_pollTimer.isActive()) return;
  if (value) {
    refresh();
    m_pollTimer.start();
  } else {
    m_pollTimer.stop();
  }
}

void MacosPermissionService::requestAccessibility() {
  const void *keys[] = {kAXTrustedCheckOptionPrompt};
  const void *values[] = {kCFBooleanTrue};
  CFDictionaryRef options =
      CFDictionaryCreate(kCFAllocatorDefault, keys, values, 1, &kCFTypeDictionaryKeyCallBacks,
                         &kCFTypeDictionaryValueCallBacks);
  AXIsProcessTrustedWithOptions(options);
  CFRelease(options);

  openPane(ACCESSIBILITY_PANE_URL);
  refresh();
}

void MacosPermissionService::requestFullDiskAccess() { openPane(FULL_DISK_PANE_URL); }

void MacosPermissionService::refresh() {
  const bool accessibility = AXIsProcessTrusted();
  if (accessibility != m_accessibilityGranted) {
    m_accessibilityGranted = accessibility;
    emit accessibilityGrantedChanged();
  }

  const bool fullDisk = probeFullDiskAccess();
  if (fullDisk != m_fullDiskAccessGranted) {
    m_fullDiskAccessGranted = fullDisk;
    emit fullDiskAccessGrantedChanged();
  }
}
