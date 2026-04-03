#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <wayland-client.h>
#include "ext-data-control-v1-client-protocol.h"
#include "wlr-data-control-unstable-v1-client-protocol.h"
#include "ext/clipman.hpp"
#include "wlr/clipman.hpp"
#include "generated/wlr-clipboard.hpp"

class StdoutTransport : public wlrclip::AbstractTransport {
  void send(std::string_view data) override {
    uint32_t size = htonl(data.size());
    std::cout.write(reinterpret_cast<const char *>(&size), sizeof(size));
    std::cout.write(data.data(), data.size());
    std::cout.flush();
  }
};

class ClipboardService : public wlrclip::AbstractClipboard {
public:
  using wlrclip::AbstractClipboard::AbstractClipboard;
};

struct ProtocolState {
  bool hasExtDataControl = false;
  bool hasWlrDataControl = false;
};

static void registryGlobal(void *data, wl_registry *, uint32_t, const char *interface, uint32_t) {
  auto state = static_cast<ProtocolState *>(data); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

  if (strcmp(interface, ext_data_control_manager_v1_interface.name) == 0) {
    state->hasExtDataControl = true;
  } else if (strcmp(interface, zwlr_data_control_manager_v1_interface.name) == 0) {
    state->hasWlrDataControl = true;
  }
}

static void registryGlobalRemove(void *, wl_registry *, uint32_t) {}

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

int main(int, char **) {
  const int protocol = detectProtocol();

  if (protocol == -1) {
    std::cerr
        << "No data control protocol available (tried ext-data-control-v1 and wlr-data-control-unstable-v1)"
        << std::endl;
    return 1;
  }

  StdoutTransport transport;
  wlrclip::RpcTransport rpc{transport};
  ClipboardService service{rpc};

  try {
    if (protocol == 0) {
      ExtClipman::instance(&service)->start();
    } else {
      WlrClipman::instance(&service)->start();
    }
  } catch (const std::exception &e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
