#include "linux-paste-service.hpp"
#include "services/input-server/linux-input-server.hpp"

LinuxPasteService::LinuxPasteService(LinuxInputServer &server) : m_server(server) {}

bool LinuxPasteService::supportsPaste() const {
  return m_server.isRunning() && m_server.supportsKeyInjection();
}

bool LinuxPasteService::pasteToApp(const AbstractWindowManager::AbstractWindow *window,
                                   const AbstractApplication *app) {
  m_server.injectPaste(app && app->isTerminalEmulator());
  return true;
}
