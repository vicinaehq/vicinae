#include "keyboard-macos.hpp"
#include "keyboard.hpp"

#include <array>

namespace Keyboard::macos {

namespace {

struct NamedKey {
  Qt::Key key;
  uint16_t keycode;
};

constexpr std::array<NamedKey, 35> NAMED_KEYS{{
    {Qt::Key_Space, kVK_Space},
    {Qt::Key_Return, kVK_Return},
    {Qt::Key_Enter, kVK_ANSI_KeypadEnter},
    {Qt::Key_Escape, kVK_Escape},
    {Qt::Key_Tab, kVK_Tab},
    {Qt::Key_Backspace, kVK_Delete},
    {Qt::Key_Delete, kVK_ForwardDelete},
    {Qt::Key_Home, kVK_Home},
    {Qt::Key_End, kVK_End},
    {Qt::Key_PageUp, kVK_PageUp},
    {Qt::Key_PageDown, kVK_PageDown},
    {Qt::Key_Left, kVK_LeftArrow},
    {Qt::Key_Right, kVK_RightArrow},
    {Qt::Key_Up, kVK_UpArrow},
    {Qt::Key_Down, kVK_DownArrow},
    {Qt::Key_F1, kVK_F1},
    {Qt::Key_F2, kVK_F2},
    {Qt::Key_F3, kVK_F3},
    {Qt::Key_F4, kVK_F4},
    {Qt::Key_F5, kVK_F5},
    {Qt::Key_F6, kVK_F6},
    {Qt::Key_F7, kVK_F7},
    {Qt::Key_F8, kVK_F8},
    {Qt::Key_F9, kVK_F9},
    {Qt::Key_F10, kVK_F10},
    {Qt::Key_F11, kVK_F11},
    {Qt::Key_F12, kVK_F12},
    {Qt::Key_F13, kVK_F13},
    {Qt::Key_F14, kVK_F14},
    {Qt::Key_F15, kVK_F15},
    {Qt::Key_F16, kVK_F16},
    {Qt::Key_F17, kVK_F17},
    {Qt::Key_F18, kVK_F18},
    {Qt::Key_F19, kVK_F19},
    {Qt::Key_F20, kVK_F20},
}};

} // namespace

std::optional<uint16_t> keycodeForNamedKey(Qt::Key key) {
  for (const auto &entry : NAMED_KEYS)
    if (entry.key == key) return entry.keycode;
  return std::nullopt;
}

Qt::Key namedKeyForKeycode(uint16_t keycode) {
  for (const auto &entry : NAMED_KEYS)
    if (entry.keycode == keycode) return entry.key;
  return Qt::Key_unknown;
}

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

const CFStringRef US_LAYOUT_ID = CFSTR("com.apple.keylayout.US");
const CFStringRef ABC_LAYOUT_ID = CFSTR("com.apple.keylayout.ABC");

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
  if (CFDataRef data = copyLayoutDataById(US_LAYOUT_ID)) { return data; }
  return copyLayoutDataById(ABC_LAYOUT_ID);
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
