#pragma once

#include "wlr-layer-shell-unstable-v1-client-protocol.h"
#include "common/types.hpp"
#include "ext-data-control-v1-client-protocol.h"
#include "kde-blur-client-protocol.h"
#include "ext-background-effect-v1-client-protocol.h"
#include "vicinae-hotkey-v1-client-protocol.h"
#include "keyboard-shortcuts-inhibit-unstable-v1-client-protocol.h"

#include <QSize>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
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

  /**
   * Pixel size of the current mode of the output with the given name (e.g "DP-1"), adjusted for the
   * output transform. Unlike sizes reported by Qt, this is not affected by any kind of scaling.
   */
  static std::optional<QSize> outputPixelSize(std::string_view name);

private:
  struct Output {
    wl_output *proxy = nullptr;
    uint32_t registryName = 0;
    std::string name;
    int32_t modeWidth = 0;
    int32_t modeHeight = 0;
    int32_t transform = WL_OUTPUT_TRANSFORM_NORMAL;
  };

  static Globals &instance();

  void scan(wl_display *display);
  static void handleGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
                           uint32_t version);
  static void globalRemove(void *data, struct wl_registry *registry, uint32_t name);

  static void outputGeometry(void *data, wl_output *output, int32_t x, int32_t y, int32_t physicalWidth,
                             int32_t physicalHeight, int32_t subpixel, const char *make, const char *model,
                             int32_t transform);
  static void outputMode(void *data, wl_output *output, uint32_t flags, int32_t width, int32_t height,
                         int32_t refresh);
  static void outputDone(void *data, wl_output *output);
  static void outputScale(void *data, wl_output *output, int32_t factor);
  static void outputName(void *data, wl_output *output, const char *name);
  static void outputDescription(void *data, wl_output *output, const char *description);

  constexpr static const struct wl_registry_listener m_listener = {.global = handleGlobal,
                                                                   .global_remove = globalRemove};
  constexpr static const struct wl_output_listener m_outputListener = {.geometry = outputGeometry,
                                                                       .mode = outputMode,
                                                                       .done = outputDone,
                                                                       .scale = outputScale,
                                                                       .name = outputName,
                                                                       .description = outputDescription};

  wl_registry *m_registry = nullptr;
  zwlr_layer_shell_v1 *m_layerShell = nullptr;
  ext_data_control_manager_v1 *m_dataControlManager = nullptr;
  org_kde_kwin_blur_manager *m_kwinBlur = nullptr;
  ext_background_effect_manager_v1 *m_backgroundEffect = nullptr;
  vicinae_hotkey_manager_v1 *m_hotkey = nullptr;
  zwp_keyboard_shortcuts_inhibit_manager_v1 *m_shortcutInhibit = nullptr;
  std::vector<std::unique_ptr<Output>> m_outputs;
};
} // namespace Wayland
