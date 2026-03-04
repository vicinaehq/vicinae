#include <linux/input-event-codes.h>
#include "linux-paste-service.hpp"

LinuxPasteService::LinuxPasteService() : m_keyboard(std::make_unique<linuxutils::UInputKeyboard>()) {}

bool LinuxPasteService::supportsPaste() const { return !m_keyboard->error().has_value(); }

bool LinuxPasteService::pasteToApp(const AbstractWindowManager::AbstractWindow *window,
                                   const AbstractApplication *app) {
  using Mod = linuxutils::UInputKeyboard::Modifier;

  if (app && app->isTerminalEmulator()) {
    m_keyboard->sendKey(KEY_V, static_cast<int>(Mod::Ctrl | Mod::Shift));
  } else {
    m_keyboard->sendKey(KEY_V, static_cast<int>(Mod::Ctrl));
  }

  return true;
}
