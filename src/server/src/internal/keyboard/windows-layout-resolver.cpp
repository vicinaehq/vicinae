#include "windows-layout-resolver.hpp"
#include <QChar>
#include <windows.h>

namespace Keyboard {

Qt::Key WindowsLayoutResolver::unshift(Qt::Key key, quint32 scanCode) {
  if (!scanCode) return key;

  const UINT vk = MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX);
  if (!vk) return key;

  const UINT mapped = MapVirtualKeyW(vk, MAPVK_VK_TO_CHAR);
  if (!mapped || (mapped & 0x80000000u)) return key;

  const QChar ch(static_cast<char16_t>(mapped & 0xFFFF));
  if (!ch.isPrint() || ch.isSpace()) return key;

  return static_cast<Qt::Key>(ch.toUpper().unicode());
}

} // namespace Keyboard
