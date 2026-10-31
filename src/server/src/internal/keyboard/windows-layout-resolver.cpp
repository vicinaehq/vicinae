#include "windows-layout-resolver.hpp"
#include <array>
#include <QChar>
#include <windows.h>

namespace Keyboard {

namespace {

std::optional<Qt::Key> keyForChar(wchar_t wc) {
  const QChar ch(static_cast<char16_t>(wc));
  if (!ch.isPrint() || ch.isSpace()) return std::nullopt;
  return static_cast<Qt::Key>(ch.toUpper().unicode());
}

} // namespace

KeyLevels WindowsLayoutResolver::levels(Qt::Key, quint32 scanCode) {
  if (!scanCode) return {};

  const UINT vk = MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX);
  if (!vk) return {};

  KeyLevels levels;

  const UINT mapped = MapVirtualKeyW(vk, MAPVK_VK_TO_CHAR);
  if (mapped && !(mapped & 0x80000000U)) levels.base = keyForChar(static_cast<wchar_t>(mapped & 0xFFFF));

  std::array<BYTE, 256> state{};
  state[VK_SHIFT] = 0x80;
  std::array<wchar_t, 4> buffer{};
  // bit 2 leaves the thread's dead-key state untouched
  if (ToUnicode(vk, scanCode, state.data(), buffer.data(), static_cast<int>(buffer.size()), 0x4) == 1) {
    levels.shifted = keyForChar(buffer[0]);
  }

  return levels;
}

} // namespace Keyboard
