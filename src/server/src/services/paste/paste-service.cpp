#include <QTimer>
#include "paste-service.hpp"
#include "vicinae.hpp"

static constexpr int FOCUS_POLL_INTERVAL_MS = 5;
static constexpr int FOCUS_POLL_MAX = 100; // 500ms max
static constexpr int POST_FOCUS_DELAY_MS = 30;
static constexpr int BLIND_PASTE_DELAY_MS = 150;

PasteService::PasteService(ClipboardService &clipboard, WindowManager &wm, AppService &appDb,
                           std::unique_ptr<AbstractPasteService> platform)
    : m_clipboard(clipboard), m_wm(wm), m_appDb(appDb), m_platform(std::move(platform)) {
  m_focusPollTimer.setInterval(FOCUS_POLL_INTERVAL_MS);
  connect(&m_focusPollTimer, &QTimer::timeout, this, &PasteService::waitForFocusAndPaste);
}

bool PasteService::supportsPaste() const { return m_platform->supportsPaste(); }

bool PasteService::pasteContent(const Clipboard::Content &content, const Clipboard::CopyOptions options) {
  if (!m_clipboard.copyContent(content, options)) return false;

  if (!m_platform->supportsPaste()) {
    qWarning() << "pasteContent called but the current platform cannot paste, ignoring...";
    return false;
  }

  // Cancel any in-flight paste
  m_focusPollTimer.stop();
  m_hasPendingPaste = true;

  if (m_wm.provider()->focusNullsOnLayerGrab()) {
    m_focusPollCount = 0;
    m_focusPollTimer.start();
  } else {
    QTimer::singleShot(BLIND_PASTE_DELAY_MS, this, &PasteService::executePaste);
  }

  return true;
}

void PasteService::waitForFocusAndPaste() {
  ++m_focusPollCount;

  auto window = m_wm.getFocusedWindow();
  bool const focusLanded = window && window->wmClass() != Omnicast::APP_ID;

  if (focusLanded || m_focusPollCount >= FOCUS_POLL_MAX) {
    m_focusPollTimer.stop();

    if (focusLanded) {
      qDebug() << "Paste: focus landed on" << window->wmClass() << "after"
               << m_focusPollCount * FOCUS_POLL_INTERVAL_MS << "ms";
      QTimer::singleShot(POST_FOCUS_DELAY_MS, this, &PasteService::executePaste);
    } else {
      qWarning() << "Paste: timed out waiting for focus transfer, dropping paste";
      m_hasPendingPaste = false;
    }
  }
}

void PasteService::executePaste() {
  if (!m_hasPendingPaste) return;
  m_hasPendingPaste = false;

  auto window = m_wm.getFocusedWindow();
  std::shared_ptr<AbstractApplication> app;

  if (window) { app = m_appDb.find(window->wmClass()); }

  if (window) {
    qInfo() << "Pasting to" << window->title() << "(" << window->wmClass() << ")"
            << (app ? QString("app: %1").arg(app->displayName()) : "");
  } else {
    qInfo() << "Pasting to unknown window";
  }

  m_platform->pasteToApp(window.get(), app.get());
}
