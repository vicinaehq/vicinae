#include "registry.hpp"

void WaylandRegistry::Listener::global(WaylandRegistry &registry, uint32_t name, const char *interface,
                                       uint32_t version) {}
void WaylandRegistry::Listener::globalRemove(struct wl_registry *registry, uint32_t name) {}

void WaylandRegistry::handleGlobal(void *data, struct wl_registry *registry, uint32_t name,
                                   const char *interface, uint32_t version) {
  auto self = static_cast<WaylandRegistry *>(data);

  for (auto lstn : self->listeners) {
    lstn->global(*self, name, interface, version);
  }
}

void WaylandRegistry::globalRemove(void *data, struct wl_registry *registry, uint32_t name) {
  auto self = static_cast<WaylandRegistry *>(data);

  for (auto lstn : self->listeners) {
    lstn->globalRemove(registry, name);
  }
}

WaylandRegistry::WaylandRegistry(wl_registry *registry) : _registry(registry) {
  wl_registry_add_listener(_registry, &_listener, this);
}

WaylandRegistry::~WaylandRegistry() {
  if (_registry) { wl_registry_destroy(_registry); }
}

void WaylandRegistry::addListener(Listener *lstn) { listeners.push_back(lstn); }
