#pragma once
#include "common/types.hpp"
#include "ext-data-control-v1-client-protocol.h"
#include "kde-blur-client-protocol.h"
#include "ext-background-effect-v1-client-protocol.h"
#include <wayland-client.h>

namespace Wayland {

class Globals : NonCopyable {
public:
  static auto kwinBlur() { return instance().m_kwinBlur; }
  static auto *extBackgroundEffectManager() { return instance().m_backgroundEffect; }
  static ext_data_control_manager_v1 *dataControlManager();

private:
  static Globals &instance();

  void scan(wl_display *display);
  static void handleGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
                           uint32_t version);
  static void globalRemove(void *data, struct wl_registry *registry, uint32_t name);

  constexpr static const struct wl_registry_listener m_listener = {.global = handleGlobal,
                                                                   .global_remove = globalRemove};

  ext_data_control_manager_v1 *m_dataControlManager = nullptr;
  org_kde_kwin_blur_manager *m_kwinBlur = nullptr;
  ext_background_effect_manager_v1 *m_backgroundEffect = nullptr;
};
} // namespace Wayland
