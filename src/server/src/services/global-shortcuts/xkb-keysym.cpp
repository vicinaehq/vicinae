#include "services/global-shortcuts/xkb-keysym.hpp"
#include <xkbcommon/xkbcommon-keysyms.h>

namespace global_shortcuts {

std::optional<uint32_t> xkbKeysymForQtKey(Qt::Key key) {
  const int k = static_cast<int>(key);

  if (k >= Qt::Key_A && k <= Qt::Key_Z) { return static_cast<uint32_t>(XKB_KEY_a + (k - Qt::Key_A)); }
  if (k >= 0x20 && k <= 0xff) { return static_cast<uint32_t>(k); } // Latin-1 maps 1:1 to xkb keysyms
  if (k >= Qt::Key_F1 && k <= Qt::Key_F35) { return static_cast<uint32_t>(XKB_KEY_F1 + (k - Qt::Key_F1)); }

  switch (key) {
  case Qt::Key_Return:
    return XKB_KEY_Return;
  case Qt::Key_Enter:
    return XKB_KEY_KP_Enter;
  case Qt::Key_Escape:
    return XKB_KEY_Escape;
  case Qt::Key_Tab:
    return XKB_KEY_Tab;
  case Qt::Key_Backspace:
    return XKB_KEY_BackSpace;
  case Qt::Key_Delete:
    return XKB_KEY_Delete;
  case Qt::Key_Insert:
    return XKB_KEY_Insert;
  case Qt::Key_Home:
    return XKB_KEY_Home;
  case Qt::Key_End:
    return XKB_KEY_End;
  case Qt::Key_PageUp:
    return XKB_KEY_Page_Up;
  case Qt::Key_PageDown:
    return XKB_KEY_Page_Down;
  case Qt::Key_Left:
    return XKB_KEY_Left;
  case Qt::Key_Right:
    return XKB_KEY_Right;
  case Qt::Key_Up:
    return XKB_KEY_Up;
  case Qt::Key_Down:
    return XKB_KEY_Down;
  default:
    return std::nullopt;
  }
}

} // namespace global_shortcuts
