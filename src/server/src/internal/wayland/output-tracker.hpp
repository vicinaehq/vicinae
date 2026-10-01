#pragma once

#include <QSize>
#include <optional>
#include <string_view>

namespace Wayland {

/**
 * Pixel size of the current mode of the output with the given name (e.g "DP-1"), adjusted for the
 * output transform. Unlike sizes reported by Qt, this is not affected by any kind of scaling.
 *
 * This intentionally stays on the raw wayland API: wl_output is a core global owned by the QPA and
 * QWaylandClientExtension only deals with extension globals.
 */
std::optional<QSize> outputPixelSize(std::string_view name);

} // namespace Wayland
