import org.kde.layershell as LayerShell
import Vicinae

LauncherWindow {
    shadowPadding: WindowMaterial.supportsRegionalBlur ? Config.shadowSize : 0

    LayerShell.Window.anchors: LayerShell.Window.AnchorNone
    LayerShell.Window.scope: "vicinae"
    LayerShell.Window.wantsToBeOnActiveScreen: true
    LayerShell.Window.layer: launcher.lsLayer
    LayerShell.Window.keyboardInteractivity: launcher.lsKeyboardInteractivity
}
