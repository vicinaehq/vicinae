#include "kde-blur-client-protocol.h"
#include <cstdint>
#include <functional>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>

namespace Wayland {
class GlobalBinder {
  static void handleGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
                           uint32_t version) {}

  static void globalRemove(void *data, struct wl_registry *registry, uint32_t name) {}

  constexpr static const struct wl_registry_listener m_listener = {.global = handleGlobal,
                                                                   .global_remove = globalRemove};

public:
  template <typename T> void bind(const wl_interface &iface, std::function<void(T *)> cb) {
    m_binds.emplace_back(BindDeclaration(iface.name, iface.version,
                                         [cb](void *global) { cb(reinterpret_cast<T *>(global)); }));
  }

  void roundtrip(wl_display *display, wl_registry *registry) {
    wl_registry_add_listener(registry, &m_listener, this);
    wl_display_roundtrip(display);
	wl_registry_
  }

private:
  struct BindDeclaration {
    const char *name;
    uint32_t version;
    std::function<void(void *ptr)> cb;
  };

  std::vector<BindDeclaration> m_binds;
};

static void test() {
  org_kde_kwin_blur_manager *manager;

  GlobalBinder binder;

  binder.bind<org_kde_kwin_blur_manager>(org_kde_kwin_blur_manager_interface,
                                         [](org_kde_kwin_blur_manager *manager) {});
}
} // namespace Wayland
