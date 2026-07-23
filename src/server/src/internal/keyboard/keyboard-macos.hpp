#pragma once
// Keep AssertMacros from defining bare check()/verify()/require(), which collide with Qt and the STL.
#define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
#include <Carbon/Carbon.h>

#include <QString>

// Layout helpers shared by recorder normalization and the global shortcut backend, so both
// sides translate identically.
namespace Keyboard::macos {

// Lowercased character(s) the key types under `layoutData` (kTISPropertyUnicodeKeyLayoutData bytes).
QString translateKeycode(CFDataRef layoutData, uint16_t keycode, bool shifted, uint8_t kbdType);

// Caller releases.
CFDataRef copyCurrentLayoutData();

// US QWERTY, ABC when US is not installed. Caller releases.
CFDataRef copyQwertyLayoutData();

} // namespace Keyboard::macos
