#include <optional>
#include <wayland-client.h>
#include "ext-image-copy-capture.hpp"
#include "internal/wayland/output-tracker.hpp"
#include "services/screen-capture/wayland/wayland-capture-job.hpp"

namespace {

constexpr int MANAGER_VERSION = 1;

class Job : public WaylandCaptureJob {
public:
  Job(struct ::ext_image_capture_source_v1 *source, struct ::ext_image_copy_capture_session_v1 *session,
      wl_shm *shm, QObject *parent)
      : WaylandCaptureJob(shm, parent), m_source(source), m_session(*this, session), m_frame(*this) {}

  ~Job() override {
    if (m_frame.isInitialized()) m_frame.destroy();
    m_session.destroy();
    ext_image_capture_source_v1_destroy(m_source);
  }

private:
  class Session : public QtWayland::ext_image_copy_capture_session_v1 {
  public:
    Session(Job &job, struct ::ext_image_copy_capture_session_v1 *session)
        : QtWayland::ext_image_copy_capture_session_v1(session), m_job(job) {}

  protected:
    void ext_image_copy_capture_session_v1_buffer_size(uint32_t width, uint32_t height) override {
      m_layout.width = static_cast<int>(width);
      m_layout.height = static_cast<int>(height);
    }

    void ext_image_copy_capture_session_v1_shm_format(uint32_t format) override {
      if (m_bytesPerPixel) return;
      m_bytesPerPixel = Wayland::ShmBuffer::bytesPerPixel(format);
      if (m_bytesPerPixel) m_layout.format = format;
    }

    void ext_image_copy_capture_session_v1_done() override {
      auto &frame = m_job.m_frame;
      // sent again whenever the constraints change, which no longer matters once we asked for our frame
      if (frame.isInitialized()) return;
      if (!m_bytesPerPixel) {
        m_job.fail("The compositor did not offer a pixel format we can read.");
        return;
      }

      m_layout.stride = m_layout.width * *m_bytesPerPixel;
      auto *buffer = m_job.allocate(m_layout);
      if (!buffer) return;

      frame.init(create_frame());
      frame.attach_buffer(buffer);
      frame.damage_buffer(0, 0, m_layout.width, m_layout.height);
      frame.capture();
    }

    void ext_image_copy_capture_session_v1_stopped() override {
      m_job.fail("The compositor stopped the capture.");
    }

  private:
    Job &m_job;
    Wayland::ShmBuffer::Layout m_layout;
    std::optional<int> m_bytesPerPixel;
  };

  class Frame : public QtWayland::ext_image_copy_capture_frame_v1 {
  public:
    explicit Frame(Job &job) : m_job(job) {}

  protected:
    void ext_image_copy_capture_frame_v1_transform(uint32_t transform) override {
      m_transform = static_cast<std::int32_t>(transform);
    }
    void ext_image_copy_capture_frame_v1_ready() override { m_job.succeed(m_transform, false); }
    void ext_image_copy_capture_frame_v1_failed(uint32_t) override {
      m_job.fail("The compositor could not capture the screen.");
    }

  private:
    Job &m_job;
    std::int32_t m_transform = WL_OUTPUT_TRANSFORM_NORMAL;
  };

  struct ::ext_image_capture_source_v1 *m_source;
  Session m_session;
  Frame m_frame;
};

} // namespace

ExtOutputImageCaptureSourceManager::ExtOutputImageCaptureSourceManager()
    : QWaylandClientExtensionTemplate(MANAGER_VERSION) {
  initialize();
}

ExtOutputImageCaptureSourceManager::~ExtOutputImageCaptureSourceManager() {
  if (isInitialized()) destroy();
}

ExtImageCopyCaptureManager::ExtImageCopyCaptureManager() : QWaylandClientExtensionTemplate(MANAGER_VERSION) {
  initialize();
}

ExtImageCopyCaptureManager::~ExtImageCopyCaptureManager() {
  if (isInitialized()) destroy();
}

bool ExtImageCopyCapture::isSupported() const {
  return m_sources.isInitialized() && m_manager.isInitialized() && m_shm.object() != nullptr;
}

QFuture<AbstractScreenCapture::Result> ExtImageCopyCapture::capture(const QScreen &screen) {
  const auto output = Wayland::findOutput(screen.name().toStdString());
  if (!output)
    return QtFuture::makeReadyValueFuture<Result>(
        std::unexpected(std::string("The screen has no matching output.")));

  auto *source = m_sources.create_source(output->proxy);
  const auto *job = new Job(source, m_manager.create_session(source, 0), m_shm.object(), this);
  return job->future();
}
