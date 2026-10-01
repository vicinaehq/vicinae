#include "output-tracker.hpp"
#include <QGuiApplication>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <wayland-client.h>

namespace {

// wl_output.name was added in version 4
constexpr uint32_t MAX_OUTPUT_VERSION = 4;

class OutputTracker {
public:
  static OutputTracker &instance() {
    static OutputTracker instance;

    if (!instance.m_registry) {
      auto *app = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
      if (app) { instance.scan(app->display()); }
    }

    return instance;
  }

  struct Output {
    wl_output *proxy = nullptr;
    uint32_t registryName = 0;
    std::string name;
    int32_t modeWidth = 0;
    int32_t modeHeight = 0;
    int32_t transform = WL_OUTPUT_TRANSFORM_NORMAL;
  };

  std::vector<std::unique_ptr<Output>> m_outputs;

private:
  void scan(wl_display *display) {
    m_registry = wl_display_get_registry(display);
    wl_registry_add_listener(m_registry, &m_listener, this);
    wl_display_roundtrip(display);
  }

  // NOLINTBEGIN(cppcoreguidelines-pro-type-static-cast-downcast)
  static void handleGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
                           uint32_t version) {
    auto *self = static_cast<OutputTracker *>(data);

    if (strcmp(interface, wl_output_interface.name) == 0) {
      auto &output = self->m_outputs.emplace_back(std::make_unique<Output>());
      output->registryName = name;
      output->proxy = static_cast<wl_output *>(
          wl_registry_bind(registry, name, &wl_output_interface, std::min(MAX_OUTPUT_VERSION, version)));
      wl_output_add_listener(output->proxy, &m_outputListener, output.get());
    }
  }

  static void globalRemove(void *data, struct wl_registry *registry, uint32_t name) {
    auto *self = static_cast<OutputTracker *>(data);
    auto it = std::ranges::find_if(self->m_outputs, [name](auto &out) { return out->registryName == name; });

    if (it != self->m_outputs.end()) {
      wl_output_destroy((*it)->proxy);
      self->m_outputs.erase(it);
    }
  }

  static void outputGeometry(void *data, wl_output *output, int32_t x, int32_t y, int32_t physicalWidth,
                             int32_t physicalHeight, int32_t subpixel, const char *make, const char *model,
                             int32_t transform) {
    static_cast<Output *>(data)->transform = transform;
  }

  static void outputMode(void *data, wl_output *output, uint32_t flags, int32_t width, int32_t height,
                         int32_t refresh) {
    if (!(flags & WL_OUTPUT_MODE_CURRENT)) return;
    auto *out = static_cast<Output *>(data);
    out->modeWidth = width;
    out->modeHeight = height;
  }

  static void outputName(void *data, wl_output *output, const char *name) {
    static_cast<Output *>(data)->name = name;
  }
  // NOLINTEND(cppcoreguidelines-pro-type-static-cast-downcast)

  static void outputDone(void *data, wl_output *output) {}
  static void outputScale(void *data, wl_output *output, int32_t factor) {}
  static void outputDescription(void *data, wl_output *output, const char *description) {}

  constexpr static const struct wl_registry_listener m_listener = {.global = handleGlobal,
                                                                   .global_remove = globalRemove};
  constexpr static const struct wl_output_listener m_outputListener = {.geometry = outputGeometry,
                                                                       .mode = outputMode,
                                                                       .done = outputDone,
                                                                       .scale = outputScale,
                                                                       .name = outputName,
                                                                       .description = outputDescription};

  wl_registry *m_registry = nullptr;
};

} // namespace

namespace Wayland {

std::optional<QSize> outputPixelSize(std::string_view name) {
  auto &tracker = OutputTracker::instance();
  auto it = std::ranges::find_if(tracker.m_outputs, [&](const auto &out) { return out->name == name; });

  if (it == tracker.m_outputs.end()) return std::nullopt;

  const auto &out = **it;

  if (out.modeWidth <= 0 || out.modeHeight <= 0) return std::nullopt;

  switch (out.transform) {
  case WL_OUTPUT_TRANSFORM_90:
  case WL_OUTPUT_TRANSFORM_270:
  case WL_OUTPUT_TRANSFORM_FLIPPED_90:
  case WL_OUTPUT_TRANSFORM_FLIPPED_270:
    return QSize(out.modeHeight, out.modeWidth);
  default:
    return QSize(out.modeWidth, out.modeHeight);
  }
}

} // namespace Wayland
