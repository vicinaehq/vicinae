#include "linux-paste-service.hpp"

LinuxPasteService::LinuxPasteService(AbstractKeyboardService &keyboard) : m_keyboard(keyboard) {}

bool LinuxPasteService::supportsPaste() const { return m_keyboard.supportsKeyInjection(); }

bool LinuxPasteService::pasteToApp(const AbstractWindowManager::AbstractWindow *window,
                                   const AbstractApplication *app) {
  m_keyboard.paste(app && app->isTerminalEmulator());
  return true;
}
