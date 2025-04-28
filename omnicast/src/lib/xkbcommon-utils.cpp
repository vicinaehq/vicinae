#include "lib/xkbcommon-utils.hpp"

// Although QT maintains a map of keys for each platform internally
// it does not expose these in any shape or form, for some reason.
// Therefore we define our own mappings for every key we are concerned
// with, for use with specific API providers.

// clang-format off
static constexpr int keyTable[]{
    Qt::Key_C, XKB_KEY_C,
    Qt::Key_V, XKB_KEY_V
};
// clang-format on

static constexpr int keyCount = sizeof(keyTable) / sizeof(*keyTable) / 2;

int XKBCommon::fromQtKey(Qt::Key key) {
  for (int i = 0; i != keyCount; ++i) {
    if (key == keyTable[i * 2]) return keyTable[i * 2 + 1];
  }

  return 0;
}

Qt::Key XKBCommon::toQtKey(int xkbKeyCode) {
  for (int i = 0; i != keyCount; ++i) {
    if (xkbKeyCode == keyTable[i * 2 + 1]) return static_cast<Qt::Key>(keyTable[i * 2]);
  }

  return {};
}
