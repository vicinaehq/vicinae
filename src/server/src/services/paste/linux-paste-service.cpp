#include "linux-paste-service.hpp"
#include "services/input-server/linux-input-server.hpp"

LinuxPasteService::LinuxPasteService(LinuxInputServer &server) : m_server(server) {}

bool LinuxPasteService::supportsPaste() const {
  return m_server.isRunning() && m_server.supportsKeyInjection();
}

bool LinuxPasteService::pasteToApp(const AbstractWindowManager::AbstractWindow *window,
                                   const AbstractApplication *app) {
  const bool isTerminal = app && (app->isTerminalEmulator() || app->isTerminalApp());

  m_server.injectPaste(isTerminal);

  return true;
}
