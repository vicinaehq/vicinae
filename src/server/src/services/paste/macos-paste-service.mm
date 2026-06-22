#include "macos-paste-service.hpp"
#include <ApplicationServices/ApplicationServices.h>

namespace {

constexpr CGKeyCode VK_ANSI_V = 0x09;

void postKey(CGKeyCode key, bool down, CGEventFlags flags) {
  CGEventRef event = CGEventCreateKeyboardEvent(nullptr, key, down);
  if (!event) { return; }
  CGEventSetFlags(event, flags);
  CGEventPost(kCGHIDEventTap, event);
  CFRelease(event);
}

} // namespace

MacosPasteService::MacosPasteService() {
  const void *keys[] = {kAXTrustedCheckOptionPrompt};
  const void *values[] = {kCFBooleanTrue};
  CFDictionaryRef options = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 1,
                                               &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  AXIsProcessTrustedWithOptions(options);
  CFRelease(options);
}

bool MacosPasteService::supportsPaste() const { return AXIsProcessTrusted(); }

bool MacosPasteService::pasteToApp(const AbstractWindowManager::AbstractWindow *,
                                   const AbstractApplication *) {
  postKey(VK_ANSI_V, true, kCGEventFlagMaskCommand);
  postKey(VK_ANSI_V, false, kCGEventFlagMaskCommand);
  return true;
}
