LauncherWindow {
    readonly property real placementFraction: 1 / 3

    nativeChrome: true
    color: "transparent"
    shadowPadding: 0
    flags: Qt.Tool | Qt.FramelessWindowHint
    autoPlaceOnShow: false

    onAboutToShow: MacOSPanel.beginShow(placementFraction)
    onShown: MacOSPanel.finishShow(placementFraction)

    MacOSWindow.enabled: true
    MacOSWindow.cornerRadius: cornerRadius
    MacOSWindow.blurEnabled: blurEnabled
    MacOSWindow.material: Config.windowStyle === "liquid_glass" ? "liquidGlass" : "hud"
    MacOSWindow.borderColor: Theme.mainWindowBorder
    MacOSWindow.borderWidth: Config.borderWidth

    MacOSPanel.enabled: true
    MacOSPanel.windowLevel: MacOSPanel.Status
    MacOSPanel.onResignKey: Nav.closeWindow()
}
