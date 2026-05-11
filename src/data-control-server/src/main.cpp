#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <wayland-client.h>
#include <glaze/glaze.hpp>
#include "ext-data-control-v1-client-protocol.h"
#include "ext/clipman.hpp"
#include "clipboard-protocol.hpp"

static void writeSelection(const clipboard_proto::Selection &selection) {
  std::string buf;
  if (auto err = glz::write_beve(selection, buf)) {
    std::cerr << "Failed to serialize clipboard selection" << std::endl;
    return;
  }

  uint8_t tag = static_cast<uint8_t>(clipboard_proto::Command::SelectionNotification);
  uint32_t size = htonl(static_cast<uint32_t>(buf.size() + 1));
  std::cout.write(reinterpret_cast<const char *>(&size), sizeof(size));
  std::cout.write(reinterpret_cast<const char *>(&tag), 1);
  std::cout.write(buf.data(), buf.size());
  std::cout.flush();
}

static bool detectProtocol() {
  wl_display *display = wl_display_connect(nullptr);

  if (!display) {
    std::cerr << "Failed to connect to Wayland display" << std::endl;
    return false;
  }

  wl_registry *registry = wl_display_get_registry(display);
  if (!registry) {
    wl_display_disconnect(display);
    std::cerr << "Failed to get Wayland registry" << std::endl;
    return false;
  }

  bool found = false;

  wl_registry_listener listener = {
      .global =
          [](void *data, wl_registry *, uint32_t, const char *interface, uint32_t) {
            if (strcmp(interface, ext_data_control_manager_v1_interface.name) == 0) {
              *static_cast<bool *>(data) = true;
            }
          },
      .global_remove = [](void *, wl_registry *, uint32_t) {},
  };

  wl_registry_add_listener(registry, &listener, &found);
  wl_display_roundtrip(display);

  wl_registry_destroy(registry);
  wl_display_disconnect(display);

  return found;
}

int main(int, char **) {
  if (!detectProtocol()) {
    std::cerr << "ext-data-control-v1 protocol not available" << std::endl;
    return 1;
  }

  try {
    ExtClipman::instance(&writeSelection)->start();
  } catch (const std::exception &e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
