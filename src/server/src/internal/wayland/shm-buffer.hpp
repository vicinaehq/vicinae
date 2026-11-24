#pragma once

#include <QImage>
#include <QtWaylandClient/QWaylandClientExtension>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <memory>
#include <optional>
#include <string>

struct wl_buffer;
struct wl_shm;

namespace Wayland {

/**
 * Our own binding of the wl_shm global, as Qt doesn't expose the one it holds.
 */
class Shm : public QWaylandClientExtension {
public:
  Shm();
  ~Shm() override;

  const struct wl_interface *extensionInterface() const override;
  void bind(struct ::wl_registry *registry, int id, int version) override;

  wl_shm *object() const { return m_shm; }

private:
  wl_shm *m_shm = nullptr;
};

class ShmBuffer {
public:
  struct Layout {
    std::uint32_t format = 0;
    int width = 0;
    int height = 0;
    int stride = 0;
  };

  /**
   * Bytes per pixel of a wl_shm format, if it is one we can read back as an image.
   */
  static std::optional<int> bytesPerPixel(std::uint32_t format);

  static std::expected<std::unique_ptr<ShmBuffer>, std::string> create(wl_shm *shm, const Layout &layout);

  ShmBuffer(const ShmBuffer &) = delete;
  ShmBuffer &operator=(const ShmBuffer &) = delete;
  ~ShmBuffer();

  wl_buffer *object() const { return m_buffer; }

  /**
   * A copy of the current content.
   */
  QImage toImage() const;

private:
  ShmBuffer(wl_buffer *buffer, void *data, std::size_t size, const Layout &layout);

  wl_buffer *m_buffer;
  void *m_data;
  std::size_t m_size;
  Layout m_layout;
};

/**
 * Undo a wl_output.transform the compositor applied to the content of a buffer.
 */
QImage undoOutputTransform(QImage image, std::int32_t transform);

} // namespace Wayland
