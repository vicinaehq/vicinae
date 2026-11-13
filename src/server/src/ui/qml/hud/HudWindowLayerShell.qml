pragma ComponentBehavior: Bound
import QtQuick
import org.kde.layershell as LayerShell
import Vicinae

HudWindow {
    // A tooltip is a popup to QtWayland, which destroys it along with the window it considers its parent.
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    // Resizing the surface every frame makes the pill's content flicker, so it animates inside a window wide
    // enough for all of its states.
    width: dictating ? 360 : pillWidth

    LayerShell.Window.layer: LayerShell.Window.LayerTop
    LayerShell.Window.scope: "vicinae-hud"
    LayerShell.Window.anchors: LayerShell.Window.AnchorBottom
    LayerShell.Window.margins.bottom: dictating ? 16 : 96
    LayerShell.Window.wantsToBeOnActiveScreen: true
    LayerShell.Window.keyboardInteractivity: LayerShell.Window.KeyboardInteractivityNone
}
