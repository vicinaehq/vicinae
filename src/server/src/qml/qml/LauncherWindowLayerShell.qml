import org.kde.layershell as LayerShell

LauncherWindow {
    shadowPadding: WindowMaterial.supportsRegionalBlur ? Config.shadowSize : 0
    zoneMode: true

    LayerShell.Window.anchors: LayerShell.Window.AnchorTop | LayerShell.Window.AnchorBottom | LayerShell.Window.AnchorLeft | LayerShell.Window.AnchorRight
    LayerShell.Window.exclusionZone: 0
    LayerShell.Window.scope: "vicinae"
    LayerShell.Window.wantsToBeOnActiveScreen: true
    LayerShell.Window.layer: launcher.lsLayer
    LayerShell.Window.keyboardInteractivity: launcher.lsKeyboardInteractivity
}
