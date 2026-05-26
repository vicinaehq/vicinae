#include "globals.hpp"
#include "ext-background-effect-v1-client-protocol.h"
#include <QGuiApplication>

namespace Wayland {

ext_data_control_manager_v1 *Globals::dataControlManager() { return instance().m_dataControlManager; }

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
}
// NOLINTEND(cppcoreguidelines-pro-type-static-cast-downcast)

Globals &Globals::instance() {
  static Globals instance;
  auto *app = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  if (app) { instance.scan(app->display()); }
  return instance;
}

void Globals::scan(wl_display *display) {
  auto registry = wl_display_get_registry(display);
  wl_registry_add_listener(registry, &m_listener, this);
  wl_display_roundtrip(display);
}

void Globals::globalRemove(void *data, struct wl_registry *registry, uint32_t name) {}

}; // namespace Wayland
