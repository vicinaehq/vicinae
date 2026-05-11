#include "linux-paste-service.hpp"
#include "services/snippet/snippet-server.hpp"

LinuxPasteService::LinuxPasteService(SnippetServer &server) : m_server(server) {}

bool LinuxPasteService::supportsPaste() const { return m_server.supportsKeyInjection(); }

bool LinuxPasteService::pasteToApp(const AbstractWindowManager::AbstractWindow *window,
                                   const AbstractApplication *app) {
  m_server.injectPaste(app && app->isTerminalEmulator());
  return true;
}
