#include "services/layout-switch/macos-layout-switch-service.hpp"

// Keep AssertMacros from defining bare check()/verify()/require(), which collide with Qt and the STL.
#define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
#include <Carbon/Carbon.h>

#include <utility>

namespace {

QString inputSourceId(TISInputSourceRef source) {
  auto id = static_cast<CFStringRef>(TISGetInputSourceProperty(source, kTISPropertyInputSourceID));
  return id ? QString::fromCFString(id) : QString();
}

TISInputSourceRef copyEnabledSourceById(const QString &id) {
  CFStringRef cfId = id.toCFString();
  CFMutableDictionaryRef filter = CFDictionaryCreateMutable(
      kCFAllocatorDefault, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  CFDictionarySetValue(filter, kTISPropertyInputSourceID, cfId);
  CFArrayRef list = TISCreateInputSourceList(filter, false);
  CFRelease(filter);
  CFRelease(cfId);
  if (!list) { return nullptr; }

  TISInputSourceRef source = nullptr;
  if (CFArrayGetCount(list) > 0) {
    source = static_cast<TISInputSourceRef>(const_cast<void *>(CFArrayGetValueAtIndex(list, 0)));
    CFRetain(source);
  }
  CFRelease(list);
  return source;
}

QString currentInputSourceId() {
  TISInputSourceRef current = TISCopyCurrentKeyboardInputSource();
  if (!current) { return {}; }

  const QString id = inputSourceId(current);
  CFRelease(current);
  return id;
}

} // namespace

std::vector<InputLayout> MacosLayoutSwitchService::availableLayouts() const {
  CFMutableDictionaryRef filter = CFDictionaryCreateMutable(
      kCFAllocatorDefault, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  CFDictionarySetValue(filter, kTISPropertyInputSourceType, kTISTypeKeyboardLayout);
  CFDictionarySetValue(filter, kTISPropertyInputSourceIsEnabled, kCFBooleanTrue);
  CFArrayRef list = TISCreateInputSourceList(filter, false);
  CFRelease(filter);

  std::vector<InputLayout> layouts;
  if (!list) { return layouts; }

  const CFIndex count = CFArrayGetCount(list);
  layouts.reserve(static_cast<size_t>(count));
  for (CFIndex i = 0; i < count; ++i) {
    auto source = static_cast<TISInputSourceRef>(const_cast<void *>(CFArrayGetValueAtIndex(list, i)));
    const QString id = inputSourceId(source);
    if (id.isEmpty()) { continue; }

    auto name = static_cast<CFStringRef>(TISGetInputSourceProperty(source, kTISPropertyLocalizedName));
    layouts.emplace_back(InputLayout{.id = id, .name = name ? QString::fromCFString(name) : id});
  }
  CFRelease(list);
  return layouts;
}

void MacosLayoutSwitchService::activate(const QString &layoutId) {
  const QString currentId = currentInputSourceId();
  if (currentId.isEmpty() || currentId == layoutId) { return; }

  TISInputSourceRef target = copyEnabledSourceById(layoutId);
  if (!target) { return; }

  if (TISSelectInputSource(target) == noErr) {
    m_savedLayoutId = currentId;
    m_appliedLayoutId = layoutId;
  }
  CFRelease(target);
}

void MacosLayoutSwitchService::restore() {
  if (m_savedLayoutId.isEmpty()) { return; }

  const QString saved = std::exchange(m_savedLayoutId, {});
  const QString applied = std::exchange(m_appliedLayoutId, {});

  // the user switched layouts while the launcher was open, keep their choice
  if (currentInputSourceId() != applied) { return; }

  if (TISInputSourceRef source = copyEnabledSourceById(saved)) {
    TISSelectInputSource(source);
    CFRelease(source);
  }
}
