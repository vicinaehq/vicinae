#pragma once
#include "wlr-data-control-unstable-v1-client-protocol.h"
#include <qobjectdefs.h>
#include <vector>
#include <wayland-client.h>
#include <wayland-util.h>

class WaylandRegistry {
public:
  class Listener {
  public:
    virtual void global(WaylandRegistry &registry, uint32_t name, const char *interface, uint32_t version);
    virtual void globalRemove(struct wl_registry *registry, uint32_t name);
  };

private:
  wl_registry *_registry;

  struct GlobalInfo {
    uint32_t name;
    const char *interface;
    uint32_t version
  };

  static void handleGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
                           uint32_t version) {
    static_cast<WaylandRegistry *>(data)->m_globals[interface] =
  }

  static void globalRemove(void *data, struct wl_registry *registry, uint32_t name) {}

  constexpr static const struct wl_registry_listener _listener = {.global = handleGlobal,
                                                                  .global_remove = globalRemove};

public:
  using Callback = std::function<void(uint32_t name, uint32_t version)>;
  std::vector<Callback> callbacks;

  std::unordered_map<std::string, void *> m_globals;

  void roundtrip() {}

  template <typename T> T *bind(const wl_interface &iface) {
    ptr = static_cast<T *>(wl_registry_bind(_registry, iface.name, iface, iface.version));
  }

  WaylandRegistry(wl_registry *registry) { wl_registry_add_listener(_registry, &_listener, this); }

  ~WaylandRegistry();

  void addListener(Listener *lstn);
};

class Test {
  Test() {
    WaylandRegistry registry;

    registry.bind<zwlr_data_control_device_v1>(zwlr_data_control_device_v1_interface);
  }
};
