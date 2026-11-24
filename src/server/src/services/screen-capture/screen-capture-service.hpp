#pragma once

#include <QObject>
#include "services/screen-capture/abstract-screen-capture.hpp"

class ScreenCaptureService : public QObject {
  Q_OBJECT

signals:
  /**
   * Windows of ours that shouldn't be part of the capture are expected to hide and call
   * `waitForHiddenWindow`.
   */
  void aboutToCapture(const QScreen *screen);
  void captureFinished();

public:
  using Result = AbstractScreenCapture::Result;

  explicit ScreenCaptureService(QObject *parent = nullptr);

  bool isAvailable() const { return m_backend != nullptr; }
  QFuture<Result> capture(QScreen *screen);

  void waitForHiddenWindow() { m_waitForHiddenWindow = true; }

private:
  AbstractScreenCapture *m_backend = nullptr;
  int m_captures = 0;
  bool m_waitForHiddenWindow = false;
};
