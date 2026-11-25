#include "linux-paste-service.hpp"
#include "services/input-server/linux-input-server.hpp"

LinuxPasteService::LinuxPasteService(LinuxInputServer &server) : m_server(server) {}

bool LinuxPasteService::supportsPaste() const {
  return m_server.isRunning() && m_server.supportsKeyInjection();
}

bool LinuxPasteService::pasteToApp(const AbstractWindowManager::AbstractWindow *window,
                                   const AbstractApplication *app, bool pastesAsText) {
  const bool isTerminal = app && (app->isTerminalEmulator() || app->isTerminalApp());

  // A terminal's own paste shortcut only pastes text: programs that take images or files read the clipboard
  // themselves when they receive Ctrl+V.
  m_server.injectPaste(isTerminal && pastesAsText);

  return true;
}
