#pragma once

#include <QObject>
#include <QPromise>
#include <QTimer>
#include <cstdint>
#include <memory>
#include <string>
#include "internal/wayland/shm-buffer.hpp"
#include "services/screen-capture/abstract-screen-capture.hpp"

/**
 * One capture in progress. Deletes itself once it reported the outcome.
 */
class WaylandCaptureJob : public QObject {
public:
  using Result = AbstractScreenCapture::Result;

  WaylandCaptureJob(wl_shm *shm, QObject *parent);

  QFuture<Result> future() const { return m_promise.future(); }

protected:
  /**
   * Returns nullptr after failing the job if the buffer couldn't be created.
   */
  wl_buffer *allocate(const Wayland::ShmBuffer::Layout &layout);
  void succeed(std::int32_t transform, bool yInverted);
  void fail(std::string reason);

private:
  void complete(Result result);

  wl_shm *m_shm;
  QPromise<Result> m_promise;
  std::unique_ptr<Wayland::ShmBuffer> m_buffer;
  QTimer m_timeout;
  bool m_done = false;
};
