#include "services/screenshots/screenshot-service.hpp"

#ifdef Q_OS_MACOS
#include "services/screenshots/macos/macos-screenshot-provider.hpp"
#endif

ScreenshotService::ScreenshotService(QObject *parent) : QObject(parent) {
#ifdef Q_OS_MACOS
  m_provider = new MacosScreenshotProvider(this);
#endif
  if (m_provider) {
    connect(m_provider, &AbstractScreenshotProvider::refreshed, this, [this](ScreenshotResult result) {
      m_result = std::move(result);
      m_loading = false;
      emit loadingChanged();
      emit refreshed();
    });
  }
}

void ScreenshotService::refresh() {
  if (!m_provider) return;
  if (!m_loading) {
    m_loading = true;
    emit loadingChanged();
  }
  m_provider->refresh();
}
