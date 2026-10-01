#include "windows-paste-service.hpp"
#include <vector>
#include <windows.h>

namespace {

constexpr WORD VK_V = 'V';

void appendKey(std::vector<INPUT> &inputs, WORD vk, bool down) {
  INPUT input{};
  input.type = INPUT_KEYBOARD;
  input.ki.wVk = vk;
  input.ki.wScan = static_cast<WORD>(MapVirtualKeyW(vk, MAPVK_VK_TO_VSC));
  if (!down) input.ki.dwFlags |= KEYEVENTF_KEYUP;
  inputs.push_back(input);
}

void releaseHeldModifiers(std::vector<INPUT> &inputs) {
  constexpr WORD MODIFIERS[] = {VK_LSHIFT, VK_RSHIFT, VK_LCONTROL, VK_RCONTROL,
                                VK_LMENU,  VK_RMENU,  VK_LWIN,     VK_RWIN};

  for (WORD vk : MODIFIERS) {
    if (GetAsyncKeyState(vk) & 0x8000) { appendKey(inputs, vk, false); }
  }
}

} // namespace

bool WindowsPasteService::supportsPaste() const { return true; }

bool WindowsPasteService::pasteToApp(const AbstractWindowManager::AbstractWindow *,
                                     const AbstractApplication *) {
  std::vector<INPUT> inputs;

  inputs.reserve(4);
  releaseHeldModifiers(inputs);
  appendKey(inputs, VK_CONTROL, true);
  appendKey(inputs, VK_V, true);
  appendKey(inputs, VK_V, false);
  appendKey(inputs, VK_CONTROL, false);

  UINT sent = SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));

  return sent == inputs.size();
}
