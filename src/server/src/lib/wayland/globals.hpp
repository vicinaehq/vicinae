#pragma once
#include "common/types.hpp"
#include "ext-data-control-v1-client-protocol.h"
#include "kde-blur-client-protocol.h"
#include "virtual-keyboard-unstable-v1-client-protocol.h"
#include "wlr-data-control-unstable-v1-client-protocol.h"
#include "ext-background-effect-v1-client-protocol.h"

namespace Wayland {

// binds useful wayland globals which we might need to use or check availability for
class Globals : NonCopyable {
public:
  static auto kwinBlur() { return instance().m_kwinBlur; }
  static zwlr_data_control_manager_v1 *wlrDataControlManager();
  static auto *extBackgroundEffectManager() { return instance().m_backgroundEffect; }

  /**
   * The new data control device interface, basically a stable copy of the old wlr equivalent. Should be
   * preferred if available.
   */
  static ext_data_control_manager_v1 *dataControlDeviceManager();

  static zwp_virtual_keyboard_manager_v1 *virtualKeyboardManager();

private:
  static Globals &instance();

  void scan(wl_display *display);
  static void handleGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
                           uint32_t version);
  static void globalRemove(void *data, struct wl_registry *registry, uint32_t name);

  constexpr static const struct wl_registry_listener m_listener = {.global = handleGlobal,
                                                                   .global_remove = globalRemove};

  zwlr_data_control_manager_v1 *m_zwlrDataControlDevice = nullptr;
  ext_data_control_manager_v1 *extDataControlDevice = nullptr;
  zwp_virtual_keyboard_manager_v1 *m_virtualKeyboardManager = nullptr;
  org_kde_kwin_blur_manager *m_kwinBlur = nullptr;
  ext_background_effect_manager_v1 *m_backgroundEffect = nullptr;
};
} // namespace Wayland
