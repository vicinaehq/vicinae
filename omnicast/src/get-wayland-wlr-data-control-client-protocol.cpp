extern "C" {
#include "wayland-wlr-data-control-client-protocol.h"
}

const struct wl_interface *get_wayland_wlr_data_control_manager() {
  return &zwlr_data_control_manager_v1_interface;
}
