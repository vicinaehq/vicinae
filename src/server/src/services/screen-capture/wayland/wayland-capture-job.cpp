#include <chrono>
#include "wayland-capture-job.hpp"

namespace {

// compositors only answer once they render a frame, which never happens for a screen that is turned off
constexpr std::chrono::seconds TIMEOUT{5};

} // namespace

WaylandCaptureJob::WaylandCaptureJob(wl_shm *shm, QObject *parent) : QObject(parent), m_shm(shm) {
  m_promise.start();
  m_timeout.setSingleShot(true);
  connect(&m_timeout, &QTimer::timeout, this, [this] { fail("The compositor did not answer in time."); });
  m_timeout.start(TIMEOUT);
}

wl_buffer *WaylandCaptureJob::allocate(const Wayland::ShmBuffer::Layout &layout) {
  auto buffer = Wayland::ShmBuffer::create(m_shm, layout);
  if (!buffer) {
    fail(std::move(buffer.error()));
    return nullptr;
  }
  m_buffer = std::move(*buffer);
  return m_buffer->object();
}

void WaylandCaptureJob::succeed(std::int32_t transform, bool yInverted) {
  if (!m_buffer) {
    fail("The compositor did not ask for a buffer.");
    return;
  }
  auto image = m_buffer->toImage();
  if (yInverted) image = image.flipped(Qt::Vertical);
  complete(Wayland::undoOutputTransform(std::move(image), transform));
}

void WaylandCaptureJob::fail(std::string reason) { complete(std::unexpected(std::move(reason))); }

void WaylandCaptureJob::complete(Result result) {
  if (m_done) return;
  m_done = true;
  m_timeout.stop();
  m_promise.addResult(std::move(result));
  m_promise.finish();
  deleteLater();
}
