#include <optional>
#include "internal/wayland/output-tracker.hpp"
#include "services/screen-capture/wayland/wayland-capture-job.hpp"
#include "wlr-screencopy-capture.hpp"

namespace {

constexpr int MANAGER_VERSION = 3;
// since this version the compositor announces every kind of buffer it accepts, then `buffer_done`
constexpr int BUFFER_DONE_VERSION = 3;

class Job : public WaylandCaptureJob {
public:
  Job(struct ::zwlr_screencopy_frame_v1 *frame, wl_shm *shm, std::int32_t transform, QObject *parent)
      : WaylandCaptureJob(shm, parent), m_frame(*this, frame), m_transform(transform) {}

  ~Job() override { m_frame.destroy(); }

private:
  class Frame : public QtWayland::zwlr_screencopy_frame_v1 {
  public:
    Frame(Job &job, struct ::zwlr_screencopy_frame_v1 *frame)
        : QtWayland::zwlr_screencopy_frame_v1(frame), m_job(job) {}

  protected:
    void zwlr_screencopy_frame_v1_buffer(uint32_t format, uint32_t width, uint32_t height,
                                         uint32_t stride) override {
      m_layout = Wayland::ShmBuffer::Layout{
          .format = format,
          .width = static_cast<int>(width),
          .height = static_cast<int>(height),
          .stride = static_cast<int>(stride),
      };
      if (wl_proxy_get_version(reinterpret_cast<wl_proxy *>(object())) < BUFFER_DONE_VERSION) requestCopy();
    }

    void zwlr_screencopy_frame_v1_buffer_done() override { requestCopy(); }

    void zwlr_screencopy_frame_v1_flags(uint32_t flags) override {
      m_yInverted = (flags & flags_y_invert) != 0;
    }

    void zwlr_screencopy_frame_v1_ready(uint32_t, uint32_t, uint32_t) override {
      m_job.succeed(m_job.m_transform, m_yInverted);
    }

    void zwlr_screencopy_frame_v1_failed() override {
      m_job.fail("The compositor could not capture the screen.");
    }

  private:
    void requestCopy() {
      if (!m_layout) {
        m_job.fail("The compositor did not offer a shared memory buffer.");
        return;
      }
      if (auto *buffer = m_job.allocate(*m_layout)) copy(buffer);
    }

    Job &m_job;
    std::optional<Wayland::ShmBuffer::Layout> m_layout;
    bool m_yInverted = false;
  };

  Frame m_frame;
  std::int32_t m_transform;
};

} // namespace

WlrScreencopyManager::WlrScreencopyManager() : QWaylandClientExtensionTemplate(MANAGER_VERSION) {
  initialize();
}

WlrScreencopyManager::~WlrScreencopyManager() {
  if (isInitialized()) destroy();
}

bool WlrScreencopyCapture::isSupported() const {
  return m_manager.isInitialized() && m_shm.object() != nullptr;
}

QFuture<AbstractScreenCapture::Result> WlrScreencopyCapture::capture(const QScreen &screen) {
  const auto output = Wayland::findOutput(screen.name().toStdString());
  if (!output)
    return QtFuture::makeReadyValueFuture<Result>(
        std::unexpected(std::string("The screen has no matching output.")));

  const auto *job =
      new Job(m_manager.capture_output(0, output->proxy), m_shm.object(), output->transform, this);
  return job->future();
}
