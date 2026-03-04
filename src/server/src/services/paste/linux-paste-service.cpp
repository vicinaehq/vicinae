#include <linux/input-event-codes.h>
#include "linux-paste-service.hpp"

LinuxPasteService::LinuxPasteService() {
  if (auto err = m_keyboard.error()) {
    qWarning() << "Paste support disabled:" << err->c_str()
               << "- ensure your user has write access to /dev/uinput."
               << "You may need to add your user to the 'input' group.";
  }
}

bool LinuxPasteService::supportsPaste() const { return !m_keyboard.error().has_value(); }

bool LinuxPasteService::pasteToApp(const AbstractWindowManager::AbstractWindow *window,
                                   const AbstractApplication *app) {
  using Mod = linuxutils::UInputKeyboard::Modifier;

  if (app && app->isTerminalEmulator()) {
    m_keyboard.sendKey(KEY_V, static_cast<int>(Mod::Ctrl | Mod::Shift));
  } else {
    m_keyboard.sendKey(KEY_V, static_cast<int>(Mod::Ctrl));
  }

  return true;
}
