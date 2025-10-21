#pragma once
#include <vector>
#include <wayland-client.h>

class WaylandRegistry {
public:
  class Listener {
  public:
    virtual void global(WaylandRegistry &registry, uint32_t name, const char *interface, uint32_t version);
    virtual void globalRemove(struct wl_registry *registry, uint32_t name);
  };

private:
  wl_registry *_registry;

  static void handleGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
                           uint32_t version);
  static void globalRemove(void *data, struct wl_registry *registry, uint32_t name);

  constexpr static const struct wl_registry_listener _listener = {.global = handleGlobal,
                                                                  .global_remove = globalRemove};

public:
  std::vector<Listener *> listeners;

  template <typename T> T *bind(uint32_t name, const struct wl_interface *iface, uint32_t version) {
    return static_cast<T *>(wl_registry_bind(_registry, name, iface, version));
  }

  WaylandRegistry(wl_registry *registry);
  ~WaylandRegistry();

  void addListener(Listener *lstn);
};
