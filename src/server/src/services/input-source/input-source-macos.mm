#include "services/input-source/input-source-service.hpp"
#include "service-registry.hpp"
#include "config/config.hpp"
#include <Carbon/Carbon.h>
#include <Foundation/Foundation.h>
#include <algorithm>

class MacosInputSourceService : public InputSourceService {
public:
  bool isSupported() const override { return true; }

  std::vector<InputSourceEntry> availableInputSources() const override {
    std::vector<InputSourceEntry> result;

    CFMutableDictionaryRef filter = CFDictionaryCreateMutable(
        kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(filter, kTISPropertyInputSourceCategory, kTISCategoryKeyboardInputSource);
    CFArrayRef list = TISCreateInputSourceList(filter, false);
    CFRelease(filter);

    if (!list) return result;

    CFIndex count = CFArrayGetCount(list);
    result.reserve(static_cast<std::size_t>(count));

    for (CFIndex i = 0; i < count; ++i) {
      auto source = static_cast<TISInputSourceRef>(const_cast<void *>(CFArrayGetValueAtIndex(list, i)));

      auto isSelectable = static_cast<CFBooleanRef>(
          TISGetInputSourceProperty(source, kTISPropertyInputSourceIsSelectCapable));
      if (isSelectable && !CFBooleanGetValue(isSelectable)) { continue; }

      auto idRef = static_cast<CFStringRef>(TISGetInputSourceProperty(source, kTISPropertyInputSourceID));
      auto nameRef = static_cast<CFStringRef>(TISGetInputSourceProperty(source, kTISPropertyLocalizedName));

      if (idRef && nameRef) {
        QString id = QString::fromNSString((__bridge NSString *)idRef);
        QString name = QString::fromNSString((__bridge NSString *)nameRef);

        if (std::none_of(result.begin(), result.end(),
                         [&](const InputSourceEntry &e) { return e.id == id; })) {
          result.emplace_back(InputSourceEntry{.id = id, .name = name});
        }
      }
    }

    CFRelease(list);
    return result;
  }

  QString currentInputSourceId() const override {
    TISInputSourceRef current = TISCopyCurrentKeyboardInputSource();
    if (!current) return {};

    auto idRef = static_cast<CFStringRef>(TISGetInputSourceProperty(current, kTISPropertyInputSourceID));
    QString id;
    if (idRef) { id = QString::fromNSString((__bridge NSString *)idRef); }
    CFRelease(current);
    return id;
  }

  bool selectInputSource(const QString &id) override {
    if (id.isEmpty()) return false;

    NSString *nsId = id.toNSString();
    CFStringRef targetId = (__bridge CFStringRef)nsId;

    CFMutableDictionaryRef filter = CFDictionaryCreateMutable(
        kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(filter, kTISPropertyInputSourceCategory, kTISCategoryKeyboardInputSource);
    CFArrayRef list = TISCreateInputSourceList(filter, false);
    CFRelease(filter);

    if (!list) return false;

    bool success = false;
    CFIndex count = CFArrayGetCount(list);
    for (CFIndex i = 0; i < count; ++i) {
      auto source = static_cast<TISInputSourceRef>(const_cast<void *>(CFArrayGetValueAtIndex(list, i)));
      auto idRef = static_cast<CFStringRef>(TISGetInputSourceProperty(source, kTISPropertyInputSourceID));
      if (idRef && CFStringCompare(idRef, targetId, 0) == kCFCompareEqualTo) {
        OSStatus status = TISSelectInputSource(source);
        success = (status == noErr);
        break;
      }
    }

    if (!success) {
      CFArrayRef allList = TISCreateInputSourceList(nullptr, true);
      if (allList) {
        CFIndex allCount = CFArrayGetCount(allList);
        for (CFIndex i = 0; i < allCount; ++i) {
          auto source =
              static_cast<TISInputSourceRef>(const_cast<void *>(CFArrayGetValueAtIndex(allList, i)));
          auto idRef = static_cast<CFStringRef>(TISGetInputSourceProperty(source, kTISPropertyInputSourceID));
          if (idRef && CFStringCompare(idRef, targetId, 0) == kCFCompareEqualTo) {
            OSStatus status = TISSelectInputSource(source);
            success = (status == noErr);
            break;
          }
        }
        CFRelease(allList);
      }
    }

    CFRelease(list);
    return success;
  }

  void onLauncherOpening() override {
    const auto &cfg = ServiceRegistry::instance()->config()->value();
    if (!cfg.switchInputSourceOnOpen) return;

    m_previousInputSourceId = currentInputSourceId();

    QString target = QString::fromStdString(cfg.targetInputSource.value_or(""));
    if (target.isEmpty()) {
      auto sources = availableInputSources();
      for (const auto &src : sources) {
        if (src.id.contains("ABC", Qt::CaseInsensitive) || src.id.contains("US", Qt::CaseInsensitive) ||
            src.name.contains("English", Qt::CaseInsensitive)) {
          target = src.id;
          break;
        }
      }
      if (target.isEmpty() && !sources.empty()) { target = sources.front().id; }
    }

    if (!target.isEmpty() && target != m_previousInputSourceId) { selectInputSource(target); }
  }

  void onLauncherClosing() override {
    const auto &cfg = ServiceRegistry::instance()->config()->value();
    if (!cfg.switchInputSourceOnOpen || !cfg.restoreInputSourceOnClose) {
      m_previousInputSourceId.clear();
      return;
    }

    if (!m_previousInputSourceId.isEmpty()) {
      selectInputSource(m_previousInputSourceId);
      m_previousInputSourceId.clear();
    }
  }

private:
  QString m_previousInputSourceId;
};

InputSourceService *InputSourceService::instance() {
  static MacosInputSourceService instance;
  return &instance;
}
