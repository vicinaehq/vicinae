pragma ComponentBehavior: Bound
import QtQuick
import org.kde.layershell as LayerShell
import Vicinae

HudWindow {
    LayerShell.Window.layer: LayerShell.Window.LayerTop
    LayerShell.Window.scope: "vicinae-hud"
    LayerShell.Window.anchors: LayerShell.Window.AnchorNone
    LayerShell.Window.wantsToBeOnActiveScreen: true
    LayerShell.Window.keyboardInteractivity: LayerShell.Window.KeyboardInteractivityNone
}
