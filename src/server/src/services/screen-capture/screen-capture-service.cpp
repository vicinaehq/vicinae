#include <QPointer>
#include <QPromise>
#include <QTimer>
#include <chrono>
#include <memory>
#include "services/screen-capture/screen-capture-service.hpp"

#ifdef Q_OS_LINUX
#include "environment.hpp"
#include "services/screen-capture/wayland/ext-image-copy-capture.hpp"
#include "services/screen-capture/wayland/wlr-screencopy-capture.hpp"
#endif

namespace {

// Compositors may animate a closing window, and nothing tells us when they are done.
constexpr std::chrono::milliseconds HIDDEN_WINDOW_DELAY{250};

#ifdef Q_OS_LINUX
template <typename T> AbstractScreenCapture *createIfSupported(QObject *parent) {
  auto *backend = new T(parent);
  if (backend->isSupported()) return backend;
  backend->deleteLater();
  return nullptr;
}
#endif

} // namespace

ScreenCaptureService::ScreenCaptureService(QObject *parent) : QObject(parent) {
#ifdef Q_OS_LINUX
  if (Environment::isWaylandSession()) {
    m_backend = createIfSupported<ExtImageCopyCapture>(this);
    if (!m_backend) m_backend = createIfSupported<WlrScreencopyCapture>(this);
  }
#endif
}

QFuture<ScreenCaptureService::Result> ScreenCaptureService::capture(QScreen *screen) {
  if (!m_backend || !screen)
    return QtFuture::makeReadyValueFuture<Result>(
        std::unexpected(std::string("Screen capture is not available.")));

  m_waitForHiddenWindow = false;
  ++m_captures;
  emit aboutToCapture(screen);

  const auto promise = std::make_shared<QPromise<Result>>();
  promise->start();

  const auto finish = [this, promise](Result result) {
    if (--m_captures == 0) emit captureFinished();
    promise->addResult(std::move(result));
    promise->finish();
  };
  const auto run = [this, finish, screen = QPointer<QScreen>(screen)] {
    if (!screen) {
      finish(std::unexpected(std::string("The screen is no longer connected.")));
      return;
    }
    m_backend->capture(*screen).then(this, finish);
  };

  if (m_waitForHiddenWindow)
    QTimer::singleShot(HIDDEN_WINDOW_DELAY, this, run);
  else
    run();

  return promise->future();
}
