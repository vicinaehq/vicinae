#pragma once
#include "common.hpp"
#include "ext-data-control-v1-client-protocol.h"
#include "wlr-data-control-unstable-v1-client-protocol.h"

namespace Wayland {

// binds useful wayland globals which we might need to use or check availability for
class Globals : NonCopyable {
public:
  static zwlr_data_control_manager_v1 *wlrDataControlManager();
  /**
   * The new data control device interface, basically a stable copy of the old wlr equivalent. Should be
   * preferred if available.
   */
  static ext_data_control_manager_v1 *dataControlDeviceManager();

private:
  static Globals &instance();

  void scan(wl_display *display);
  static void handleGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
                           uint32_t version);
  static void globalRemove(void *data, struct wl_registry *registry, uint32_t name);

  constexpr static const struct wl_registry_listener m_listener = {.global = handleGlobal,
                                                                   .global_remove = globalRemove};

  zwlr_data_control_manager_v1 *m_zwlr_data_control_device = nullptr;
  ext_data_control_manager_v1 *ext_data_control_device = nullptr;
};
} // namespace Wayland
