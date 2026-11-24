#include <QTransform>
#include <algorithm>
#include <array>
#include <cerrno>
#include <cstring>
#include <format>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client.h>
#include "shm-buffer.hpp"

namespace {

constexpr int MAX_EDGE = 16384;

struct Format {
  std::uint32_t shm;
  QImage::Format image;
  int bytesPerPixel;
};

// wl_shm names formats after their layout in a little-endian word, QImage's byte-ordered formats read the
// other way around.
constexpr auto FORMATS = std::to_array<Format>({
    {WL_SHM_FORMAT_XRGB8888, QImage::Format_RGB32, 4},
    {WL_SHM_FORMAT_ARGB8888, QImage::Format_RGB32, 4},
    {WL_SHM_FORMAT_XBGR8888, QImage::Format_RGBA8888, 4},
    {WL_SHM_FORMAT_ABGR8888, QImage::Format_RGBA8888, 4},
    {WL_SHM_FORMAT_XRGB2101010, QImage::Format_RGB30, 4},
    {WL_SHM_FORMAT_ARGB2101010, QImage::Format_RGB30, 4},
    {WL_SHM_FORMAT_XBGR2101010, QImage::Format_BGR30, 4},
    {WL_SHM_FORMAT_ABGR2101010, QImage::Format_BGR30, 4},
    {WL_SHM_FORMAT_BGR888, QImage::Format_RGB888, 3},
    {WL_SHM_FORMAT_RGB888, QImage::Format_BGR888, 3},
});

constexpr const Format *findFormat(std::uint32_t format) {
  const auto it = std::ranges::find(FORMATS, format, &Format::shm);
  return it != FORMATS.end() ? &*it : nullptr;
}

} // namespace

namespace Wayland {

Shm::Shm() : QWaylandClientExtension(1) { initialize(); }

Shm::~Shm() {
  if (m_shm) wl_shm_destroy(m_shm);
}

const struct wl_interface *Shm::extensionInterface() const { return &wl_shm_interface; }

void Shm::bind(struct ::wl_registry *registry, int id, int) {
  m_shm =
      static_cast<wl_shm *>(wl_registry_bind(registry, static_cast<std::uint32_t>(id), &wl_shm_interface, 1));
}

std::optional<int> ShmBuffer::bytesPerPixel(std::uint32_t format) {
  const auto *known = findFormat(format);
  return known ? std::optional(known->bytesPerPixel) : std::nullopt;
}

std::expected<std::unique_ptr<ShmBuffer>, std::string> ShmBuffer::create(wl_shm *shm, const Layout &layout) {
  const auto *format = findFormat(layout.format);
  if (!format) return std::unexpected(std::format("Unsupported pixel format {:#x}.", layout.format));
  if (layout.width <= 0 || layout.height <= 0 || layout.width > MAX_EDGE || layout.height > MAX_EDGE ||
      layout.stride < layout.width * format->bytesPerPixel || layout.stride > MAX_EDGE * 4)
    return std::unexpected(std::string("The compositor announced an invalid buffer."));

  const auto size = static_cast<std::size_t>(layout.stride) * static_cast<std::size_t>(layout.height);
  const int fd = memfd_create("vicinae-screen-capture", MFD_CLOEXEC);
  if (fd < 0) return std::unexpected(std::format("Could not allocate a buffer: {}", std::strerror(errno)));

  if (ftruncate(fd, static_cast<off_t>(size)) != 0) {
    const auto reason = std::format("Could not allocate a buffer: {}", std::strerror(errno));
    close(fd);
    return std::unexpected(reason);
  }

  void *data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    const auto reason = std::format("Could not map a buffer: {}", std::strerror(errno));
    close(fd);
    return std::unexpected(reason);
  }

  auto *pool = wl_shm_create_pool(shm, fd, static_cast<std::int32_t>(size));
  auto *buffer =
      wl_shm_pool_create_buffer(pool, 0, layout.width, layout.height, layout.stride, layout.format);
  wl_shm_pool_destroy(pool);
  close(fd);

  return std::unique_ptr<ShmBuffer>(new ShmBuffer(buffer, data, size, layout));
}

ShmBuffer::ShmBuffer(wl_buffer *buffer, void *data, std::size_t size, const Layout &layout)
    : m_buffer(buffer), m_data(data), m_size(size), m_layout(layout) {}

ShmBuffer::~ShmBuffer() {
  wl_buffer_destroy(m_buffer);
  munmap(m_data, m_size);
}

QImage ShmBuffer::toImage() const {
  const QImage view(static_cast<const uchar *>(m_data), m_layout.width, m_layout.height, m_layout.stride,
                    findFormat(m_layout.format)->image);
  // the view borrows our mapping, and converting to the format it already has would hand it back as is
  return view.format() == QImage::Format_RGB32 ? view.copy() : view.convertToFormat(QImage::Format_RGB32);
}

QImage undoOutputTransform(QImage image, std::int32_t transform) {
  if (transform < WL_OUTPUT_TRANSFORM_NORMAL || transform > WL_OUTPUT_TRANSFORM_FLIPPED_270) return image;

  // the flipped transforms come after the plain ones, in the same order
  const int quarterTurns = transform % WL_OUTPUT_TRANSFORM_FLIPPED;
  const bool flipped = transform >= WL_OUTPUT_TRANSFORM_FLIPPED;

  // the content was flipped then rotated counter-clockwise, so we rotate it back clockwise then flip it
  if (quarterTurns != 0) image = image.transformed(QTransform().rotate(90.0 * quarterTurns));
  if (flipped) image = image.flipped(Qt::Horizontal);
  return image;
}

} // namespace Wayland
