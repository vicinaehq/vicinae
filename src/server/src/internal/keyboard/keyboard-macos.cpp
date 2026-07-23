#include "keyboard-macos.hpp"
#include "keyboard.hpp"

#include <array>

namespace Keyboard::macos {

QString translateKeycode(CFDataRef layoutData, uint16_t keycode, bool shifted, uint8_t kbdType) {
  if (!layoutData) { return {}; }

  const auto *layout = reinterpret_cast<const UCKeyboardLayout *>(CFDataGetBytePtr(layoutData));
  const UInt32 modifierState = shifted ? ((shiftKey >> 8) & 0xFF) : 0;
  UInt32 deadKeyState = 0;
  std::array<UniChar, 4> chars{};
  UniCharCount length = 0;

  if (UCKeyTranslate(layout, keycode, kUCKeyActionDown, modifierState, kbdType, kUCKeyTranslateNoDeadKeysBit,
                     &deadKeyState, chars.size(), &length, chars.data()) != noErr) {
    return {};
  }

  return QString::fromUtf16(reinterpret_cast<const char16_t *>(chars.data()), static_cast<qsizetype>(length))
      .toLower();
}

namespace {

CFDataRef copyLayoutDataById(CFStringRef id) {
  CFMutableDictionaryRef filter = CFDictionaryCreateMutable(
      kCFAllocatorDefault, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  CFDictionarySetValue(filter, kTISPropertyInputSourceID, id);
  CFArrayRef list = TISCreateInputSourceList(filter, true);
  CFRelease(filter);
  if (!list) { return nullptr; }

  CFDataRef data = nullptr;
  if (CFArrayGetCount(list) > 0) {
    auto source = static_cast<TISInputSourceRef>(const_cast<void *>(CFArrayGetValueAtIndex(list, 0)));
    data = static_cast<CFDataRef>(TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData));
    if (data) { CFRetain(data); }
  }
  CFRelease(list);
  return data;
}

} // namespace

CFDataRef copyCurrentLayoutData() {
  TISInputSourceRef source = TISCopyCurrentKeyboardLayoutInputSource();
  if (!source) { return nullptr; }

  auto data = static_cast<CFDataRef>(TISGetInputSourceProperty(source, kTISPropertyUnicodeKeyLayoutData));
  if (data) { CFRetain(data); }
  CFRelease(source);
  return data;
}

CFDataRef copyQwertyLayoutData() {
  if (CFDataRef data = copyLayoutDataById(CFSTR("com.apple.keylayout.US"))) { return data; }
  return copyLayoutDataById(CFSTR("com.apple.keylayout.ABC"));
}

} // namespace Keyboard::macos

namespace {

constexpr uint16_t MAX_KEYCODE = 127;

std::optional<uint16_t> keycodeForChar(CFDataRef layoutData, const QString &character, uint8_t kbdType) {
  for (const bool shifted : {false, true}) {
    for (uint16_t keycode = 0; keycode <= MAX_KEYCODE; ++keycode) {
      if (Keyboard::macos::translateKeycode(layoutData, keycode, shifted, kbdType) == character) {
        return keycode;
      }
    }
  }
  return std::nullopt;
}

} // namespace

Qt::Key Keyboard::normalizeToLatin(Qt::Key key) {
  const auto ch = printableCharForKey(key);
  if (!ch) { return key; }
  if (ch->script() == QChar::Script_Latin || ch->script() == QChar::Script_Common) { return key; }

  const uint8_t kbdType = LMGetKbdType();

  CFDataRef current = macos::copyCurrentLayoutData();
  const auto keycode = keycodeForChar(current, QString(ch->toLower()), kbdType);
  if (current) { CFRelease(current); }
  if (!keycode) { return key; }

  CFDataRef qwerty = macos::copyQwertyLayoutData();
  const QString latin = macos::translateKeycode(qwerty, *keycode, false, kbdType);
  if (qwerty) { CFRelease(qwerty); }

  if (latin.size() != 1) { return key; }

  const QChar latinChar = latin.front().toUpper();
  if (!latinChar.isPrint() || latinChar.isSpace()) { return key; }

  return static_cast<Qt::Key>(latinChar.unicode());
}
