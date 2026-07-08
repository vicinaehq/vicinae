#include "globals.hpp"
#include "ext-background-effect-v1-client-protocol.h"
#include "vicinae-hotkey-v1-client-protocol.h"
#include "keyboard-shortcuts-inhibit-unstable-v1-client-protocol.h"
#include "wlr-layer-shell-unstable-v1-client-protocol.h"
#include <QGuiApplication>
#include <algorithm>

namespace Wayland {

// wl_output.name was added in version 4
constexpr uint32_t MAX_OUTPUT_VERSION = 4;

ext_data_control_manager_v1 *Globals::dataControlManager() { return instance().m_dataControlManager; }

std::optional<QSize> Globals::outputPixelSize(std::string_view name) {
  auto &self = instance();
  auto it = std::ranges::find_if(self.m_outputs, [&](const auto &out) { return out->name == name; });

  if (it == self.m_outputs.end() || (*it)->modeSize.isEmpty()) return std::nullopt;

  switch ((*it)->transform) {
  case WL_OUTPUT_TRANSFORM_90:
  case WL_OUTPUT_TRANSFORM_270:
  case WL_OUTPUT_TRANSFORM_FLIPPED_90:
  case WL_OUTPUT_TRANSFORM_FLIPPED_270:
    return (*it)->modeSize.transposed();
  default:
    return (*it)->modeSize;
  }
}

void Globals::outputGeometry(void *data, wl_output *output, int32_t x, int32_t y, int32_t physicalWidth,
                             int32_t physicalHeight, int32_t subpixel, const char *make, const char *model,
                             int32_t transform) {
  static_cast<Output *>(data)->transform = transform;
}

void Globals::outputMode(void *data, wl_output *output, uint32_t flags, int32_t width, int32_t height,
                         int32_t refresh) {
  if (!(flags & WL_OUTPUT_MODE_CURRENT)) return;
  static_cast<Output *>(data)->modeSize = QSize(width, height);
}

void Globals::outputName(void *data, wl_output *output, const char *name) {
  static_cast<Output *>(data)->name = name;
}

void Globals::outputDone(void *data, wl_output *output) {}
void Globals::outputScale(void *data, wl_output *output, int32_t factor) {}
void Globals::outputDescription(void *data, wl_output *output, const char *description) {}

// NOLINTBEGIN(cppcoreguidelines-pro-type-static-cast-downcast)
void Globals::handleGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
                           uint32_t version) {
  auto self = static_cast<Globals *>(data);

  if (strcmp(interface, ext_data_control_manager_v1_interface.name) == 0) {
    self->m_dataControlManager = static_cast<ext_data_control_manager_v1 *>(
        wl_registry_bind(registry, name, &ext_data_control_manager_v1_interface, version));
  }

  else if (strcmp(interface, org_kde_kwin_blur_manager_interface.name) == 0) {
    self->m_kwinBlur = static_cast<decltype(self->m_kwinBlur)>(
        wl_registry_bind(registry, name, &org_kde_kwin_blur_manager_interface, version));
  }

  else if (strcmp(interface, ext_background_effect_manager_v1_interface.name) == 0) {
    self->m_backgroundEffect = static_cast<decltype(self->m_backgroundEffect)>(
        wl_registry_bind(registry, name, &ext_background_effect_manager_v1_interface, version));
  }

  else if (strcmp(interface, vicinae_hotkey_manager_v1_interface.name) == 0) {
    self->m_hotkey = static_cast<decltype(self->m_hotkey)>(
        wl_registry_bind(registry, name, &vicinae_hotkey_manager_v1_interface, version));
  }

  else if (strcmp(interface, zwp_keyboard_shortcuts_inhibit_manager_v1_interface.name) == 0) {
    self->m_shortcutInhibit = static_cast<decltype(self->m_shortcutInhibit)>(
        wl_registry_bind(registry, name, &zwp_keyboard_shortcuts_inhibit_manager_v1_interface, version));
  }

  else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
    self->m_layerShell = static_cast<decltype(self->m_layerShell)>(
        wl_registry_bind(registry, name, &zwlr_layer_shell_v1_interface, version));
  }

  else if (strcmp(interface, wl_output_interface.name) == 0) {
    auto &output = self->m_outputs.emplace_back(std::make_unique<Output>());
    output->registryName = name;
    output->proxy = static_cast<wl_output *>(
        wl_registry_bind(registry, name, &wl_output_interface, std::min(MAX_OUTPUT_VERSION, version)));
    wl_output_add_listener(output->proxy, &m_outputListener, output.get());
    self->m_outputsChanged = true;
  }
}
// NOLINTEND(cppcoreguidelines-pro-type-static-cast-downcast)

Globals &Globals::instance() {
  static Globals instance;

  if (!instance.m_registry) {
    auto *app = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    if (app) { instance.scan(app->display()); }
  }

  return instance;
}

void Globals::scan(wl_display *display) {
  m_registry = wl_display_get_registry(display);
  wl_registry_add_listener(m_registry, &m_listener, this);
  wl_display_roundtrip(display);
  // output binds are issued while dispatching the first roundtrip, so their initial
  // events (mode, name, ...) only arrive after another one. Anything past this point
  // (hotplugs, mode changes) is dispatched by Qt's event loop on the shared display.
  if (m_outputsChanged) wl_display_roundtrip(display);
}

void Globals::globalRemove(void *data, struct wl_registry *registry, uint32_t name) {
  auto self = static_cast<Globals *>(data);
  auto it = std::ranges::find_if(self->m_outputs, [name](auto &out) { return out->registryName == name; });

  if (it != self->m_outputs.end()) {
    wl_output_destroy((*it)->proxy);
    self->m_outputs.erase(it);
  }
}

}; // namespace Wayland
