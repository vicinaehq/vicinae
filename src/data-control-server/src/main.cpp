#include <iostream>
#include <cstring>
#include <wayland-client.h>
#include "ext-data-control-v1-client-protocol.h"
#include "wlr-data-control-unstable-v1-client-protocol.h"
#include "ext/clipman.hpp"
#include "wlr/clipman.hpp"

struct ProtocolState {
  bool hasExtDataControl = false;
  bool hasWlrDataControl = false;
};

static void registryGlobal(void *data, wl_registry *registry, uint32_t name, const char *interface,
                           uint32_t version) {
  auto state = static_cast<ProtocolState *>(data);

  if (strcmp(interface, ext_data_control_manager_v1_interface.name) == 0) {
    state->hasExtDataControl = true;
  } else if (strcmp(interface, zwlr_data_control_manager_v1_interface.name) == 0) {
    state->hasWlrDataControl = true;
  }
}

static void registryGlobalRemove(void *data, wl_registry *registry, uint32_t name) {}

static const wl_registry_listener registryListener = {
    .global = registryGlobal,
    .global_remove = registryGlobalRemove,
};

static int detectProtocol() {
  wl_display *display = wl_display_connect(nullptr);

  if (!display) {
    std::cerr << "Failed to connect to Wayland display" << std::endl;
    return -1;
  }

  wl_registry *registry = wl_display_get_registry(display);
  if (!registry) {
    wl_display_disconnect(display);
    std::cerr << "Failed to get Wayland registry" << std::endl;
    return -1;
  }

  ProtocolState state;
  wl_registry_add_listener(registry, &registryListener, &state);
  wl_display_roundtrip(display);

  wl_registry_destroy(registry);
  wl_display_disconnect(display);

  if (state.hasExtDataControl) {
    return 0;
  } else if (state.hasWlrDataControl) {
    return 1;
  }

  return -1;
}

int main(int argc, char **argv) {
  int protocol = detectProtocol();

  if (protocol == -1) {
    std::cerr
        << "No data control protocol available (tried ext-data-control-v1 and wlr-data-control-unstable-v1)"
        << std::endl;
    return 1;
  }

  try {
    if (protocol == 0) {
      ExtClipman::instance()->start();
    } else {
      WlrClipman::instance()->start();
    }
  } catch (const std::exception &e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
