pragma ComponentBehavior: Bound
import QtQuick
import org.kde.layershell as LayerShell
import Vicinae

LauncherWindow {
    id: launcher

    shadowPadding: WindowMaterial.supportsRegionalBlur ? Config.shadowSize : 0

    LayerShell.Window.anchors: LayerShell.Window.AnchorNone
    LayerShell.Window.scope: "vicinae"
    LayerShell.Window.wantsToBeOnActiveScreen: true
    LayerShell.Window.layer: Launcher.lsLayer
    LayerShell.Window.keyboardInteractivity: Launcher.lsKeyboardInteractivity

    // Compositors stack the surfaces of a layer in the order they were created, so the backdrop goes first.
    onAboutToShow: backdrop.visible = Config.layerShellBackdrop
    onVisibleChanged: {
        if (!launcher.visible)
            backdrop.visible = false;
    }

    Window {
        id: backdrop

        // A transient child is only shown once its parent is, which would put it above the launcher.
        transientParent: null
        visible: false
        color: "transparent"
        flags: Qt.FramelessWindowHint | Qt.WindowDoesNotAcceptFocus

        LayerShell.Window.anchors: LayerShell.Window.AnchorTop | LayerShell.Window.AnchorBottom | LayerShell.Window.AnchorLeft | LayerShell.Window.AnchorRight
        LayerShell.Window.scope: "vicinae-backdrop"
        LayerShell.Window.wantsToBeOnActiveScreen: true
        LayerShell.Window.layer: Launcher.lsLayer
        LayerShell.Window.keyboardInteractivity: LayerShell.Window.KeyboardInteractivityNone
        // Covers panels and docks too, instead of stopping at the area they reserve.
        LayerShell.Window.exclusionZone: -1

        Rectangle {
            anchors.fill: parent
            color: Theme.mainWindowBackdrop
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
            onClicked: Launcher.nav.closeWindow()
        }
    }
}
