#include <cstdint>
#include <cstring>
#include <iostream>

#include <iostream>
#include <ostream>
#include <stdexcept>
#include <vector>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include "wayland-wlr-data-control-client-protocol.h"

class IRegistryListener {
public:
  virtual void global(struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {}
  virtual void globalRemove(struct wl_registry *registry, uint32_t name) {
    // Handle removal if needed
  }

  IRegistryListener() {}
};

class WaylandRegistry {
  wl_registry *_registry;

  static void handleGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
                           uint32_t version) {
    auto self = static_cast<WaylandRegistry *>(data);

    for (auto lstn : self->listeners) {
      lstn->global(registry, name, interface, version);
    }
  }

  static void globalRemove(void *data, struct wl_registry *registry, uint32_t name) {
    auto self = static_cast<WaylandRegistry *>(data);

    for (auto lstn : self->listeners) {
      lstn->globalRemove(registry, name);
    }
  }

  constexpr static const struct wl_registry_listener _listener = {.global = handleGlobal,
                                                                  .global_remove = globalRemove};

public:
  std::vector<IRegistryListener *> listeners;

  WaylandRegistry(wl_registry *registry) : _registry(registry) {
    wl_registry_add_listener(_registry, &_listener, this);
  }

  void addListener(IRegistryListener *lstn) { listeners.push_back(lstn); }
};

class WaylandDisplay {
  wl_display *_display;

public:
  WaylandDisplay() { _display = wl_display_connect(nullptr); }
  ~WaylandDisplay() {
    if (_display) { wl_display_disconnect(_display); }
  };

  WaylandRegistry registry() const { return wl_display_get_registry(_display); }
  int dispatch() const { return wl_display_dispatch(_display); };
  int roundtrip() const { return wl_display_roundtrip(_display); };
};

class Clipman : public IRegistryListener {
  WaylandDisplay display;
  WaylandRegistry *registry;
  zwlr_data_control_manager_v1 *zwlr;
  wl_seat *_seat;

  void global(struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) override {
    std::cerr << "global " << interface << std::endl;

    if (strcmp(interface, zwlr_data_control_manager_v1_interface.name) == 0) {
      // Bind to the interface
      zwlr = static_cast<struct zwlr_data_control_manager_v1 *>(
          wl_registry_bind(registry, name, &zwlr_data_control_manager_v1_interface, std::min(version, 1u)));
      std::cerr << "device bound" << std::endl;
    }

    if (strcmp(interface, wl_seat_interface.name) == 0) {
      _seat =
          static_cast<wl_seat *>(wl_registry_bind(registry, name, &wl_seat_interface, std::min(version, 1u)));
    }
  }

public:
  Clipman() : zwlr(nullptr), _seat(nullptr) {
    registry = new WaylandRegistry(display.registry());
    registry->addListener(this);
  }

  ~Clipman() { delete registry; }

  void run() {
    display.roundtrip();

    if (!zwlr) { throw std::runtime_error("zwlr data control is not available"); }
    if (!_seat) { throw std::runtime_error("seat is not available"); }

    auto dataControlDevice = zwlr_data_control_manager_v1_get_data_device(zwlr, _seat);

    if (!dataControlDevice) { throw std::runtime_error("Failed to get data control device"); }

    while (display.dispatch() != -1) {}
  }
};

int main() {
  Clipman clipman;

  clipman.run();
}
