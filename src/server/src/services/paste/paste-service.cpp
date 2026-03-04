#include <QTimer>
#include "environment.hpp"
#include "paste-service.hpp"

PasteService::PasteService(ClipboardService &clipboard, WindowManager &wm, AppService &appDb,
                           std::unique_ptr<AbstractPasteService> platform)
    : m_clipboard(clipboard), m_wm(wm), m_appDb(appDb), m_platform(std::move(platform)) {}

bool PasteService::supportsPaste() const { return m_platform->supportsPaste(); }

bool PasteService::pasteContent(const Clipboard::Content &content, const Clipboard::CopyOptions options) {
  if (!m_clipboard.copyContent(content, options)) return false;

  if (!m_platform->supportsPaste()) {
    qWarning() << "pasteContent called but the current platform cannot paste, ignoring...";
    return false;
  }

  QTimer::singleShot(Environment::pasteDelay(), [this]() {
    auto window = m_wm.getFocusedWindow();
    std::shared_ptr<AbstractApplication> app;
    if (window) { app = m_appDb.find(window->wmClass()); }
    m_platform->pasteToApp(window.get(), app.get());
  });

  return true;
}
