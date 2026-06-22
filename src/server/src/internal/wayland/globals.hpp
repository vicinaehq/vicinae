#pragma once

#include "wlr-layer-shell-unstable-v1-client-protocol.h"
#include "common/types.hpp"
#include "ext-data-control-v1-client-protocol.h"
#include "kde-blur-client-protocol.h"
#include "ext-background-effect-v1-client-protocol.h"
#include "vicinae-hotkey-v1-client-protocol.h"
#include "keyboard-shortcuts-inhibit-unstable-v1-client-protocol.h"

#include <wayland-client.h>

namespace Wayland {

class Globals : NonCopyable {
public:
  static auto kwinBlur() { return instance().m_kwinBlur; }
  static auto *extBackgroundEffectManager() { return instance().m_backgroundEffect; }
  static ext_data_control_manager_v1 *dataControlManager();
  static vicinae_hotkey_manager_v1 *hotkey() { return instance().m_hotkey; }
  static zwp_keyboard_shortcuts_inhibit_manager_v1 *shortcutInhibit() { return instance().m_shortcutInhibit; }
  static zwlr_layer_shell_v1 *layerShell() { return instance().m_layerShell; }

private:
  static Globals &instance();

  void scan(wl_display *display);
  static void handleGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
                           uint32_t version);
  static void globalRemove(void *data, struct wl_registry *registry, uint32_t name);

  constexpr static const struct wl_registry_listener m_listener = {.global = handleGlobal,
                                                                   .global_remove = globalRemove};

  zwlr_layer_shell_v1 *m_layerShell = nullptr;
  ext_data_control_manager_v1 *m_dataControlManager = nullptr;
  org_kde_kwin_blur_manager *m_kwinBlur = nullptr;
  ext_background_effect_manager_v1 *m_backgroundEffect = nullptr;
  vicinae_hotkey_manager_v1 *m_hotkey = nullptr;
  zwp_keyboard_shortcuts_inhibit_manager_v1 *m_shortcutInhibit = nullptr;
};
} // namespace Wayland
