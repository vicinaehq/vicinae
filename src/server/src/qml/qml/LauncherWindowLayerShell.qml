import org.kde.layershell as LayerShell

LauncherWindow {
    shadowPadding: Config.shadowSize

    LayerShell.Window.anchors: LayerShell.Window.AnchorNone
    LayerShell.Window.scope: "vicinae"
    LayerShell.Window.wantsToBeOnActiveScreen: true
    LayerShell.Window.layer: launcher.lsLayer
    LayerShell.Window.keyboardInteractivity: launcher.lsKeyboardInteractivity
}
