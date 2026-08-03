#include "layer-shell.hpp"
#include <QGuiApplication>
#include <QtWaylandClient/QWaylandClientExtension>
#include "qwayland-wlr-layer-shell-unstable-v1.h"

namespace {
class LayerShellV1 : public QWaylandClientExtensionTemplate<LayerShellV1>,
                     public QtWayland::zwlr_layer_shell_v1 {
public:
  LayerShellV1() : QWaylandClientExtensionTemplate(1) { initialize(); }
};
} // namespace

namespace Wayland {

bool hasLayerShell() {
  if (QGuiApplication::platformName() != "wayland") { return false; }

  static LayerShellV1 layerShell;
  return layerShell.isActive();
}

} // namespace Wayland
