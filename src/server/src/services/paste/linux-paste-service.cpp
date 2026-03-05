#include <linux/input-event-codes.h>
#include <qlogging.h>
#include "linux-paste-service.hpp"
#include "vicinae.hpp"

LinuxPasteService::LinuxPasteService() {
  if (const auto err = m_keyboard.error()) {
    const auto docUrl = QString("%1/paste").arg(Omnicast::DOC_URL);
    qWarning().noquote() << "Paste support disabled:" << err->c_str()
                         << "- ensure your user has write access to /dev/uinput."
                         << "You may need to add your user to the 'input' group. More information at"
                         << docUrl;
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
