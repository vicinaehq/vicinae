#include "globals.hpp"
#include "virtual-keyboard-unstable-v1-client-protocol.h"
#include <QApplication>

namespace Wayland {

zwlr_data_control_manager_v1 *Globals::wlrDataControlManager() {
  return instance().m_zwlr_data_control_device;
}

ext_data_control_manager_v1 *Globals::dataControlDeviceManager() {
  return instance().ext_data_control_device;
}

zwp_virtual_keyboard_manager_v1 *Globals::virtualKeyboardManager() {
  return instance().m_virtual_keyboard_manager;
}

void Globals::handleGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
                           uint32_t version) {
  auto self = static_cast<Globals *>(data);

  if (strcmp(interface, zwlr_data_control_manager_v1_interface.name) == 0) {
    self->m_zwlr_data_control_device = static_cast<zwlr_data_control_manager_v1 *>(
        wl_registry_bind(registry, name, &zwlr_data_control_manager_v1_interface, version));
  }

  if (strcmp(interface, ext_data_control_manager_v1_interface.name) == 0) {
    self->ext_data_control_device = static_cast<ext_data_control_manager_v1 *>(
        wl_registry_bind(registry, name, &ext_data_control_manager_v1_interface, version));
  }

  if (strcmp(interface, zwp_virtual_keyboard_manager_v1_interface.name) == 0) {
    self->m_virtual_keyboard_manager = static_cast<zwp_virtual_keyboard_manager_v1 *>(
        wl_registry_bind(registry, name, &zwp_virtual_keyboard_manager_v1_interface, version));
  }

  if (strcmp(interface, org_kde_kwin_blur_manager_interface.name) == 0) {
    self->m_kwin_blur = static_cast<decltype(self->m_kwin_blur)>(
        wl_registry_bind(registry, name, &org_kde_kwin_blur_manager_interface, version));
  }
}

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
