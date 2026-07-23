#pragma once
// Keep AssertMacros from defining bare check()/verify()/require(), which collide with Qt and the STL.
#define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
#include <Carbon/Carbon.h>

#include <QString>

// Shared keyboard-layout helpers for the shortcut recorder normalization and the global shortcut
// backend. Both sides must translate identically or recorded shortcuts drift from match-time.
namespace Keyboard::macos {

// Lowercased character(s) the key at `keycode` types under `layoutData`
// (kTISPropertyUnicodeKeyLayoutData bytes). Empty on failure or null layout.
QString translateKeycode(CFDataRef layoutData, uint16_t keycode, bool shifted, uint8_t kbdType);

// Layout data of the active keyboard layout. Caller releases.
CFDataRef copyCurrentLayoutData();

// Layout data of US QWERTY, falling back to ABC when US is not installed. Caller releases.
CFDataRef copyQwertyLayoutData();

} // namespace Keyboard::macos
