LauncherWindow {
    readonly property real placementFraction: 1 / 3

    nativeChrome: true
    color: "transparent"
    shadowPadding: 0
    flags: Qt.Tool | Qt.FramelessWindowHint
    autoPlaceOnShow: false

    height: _contentH
    minimumHeight: _contentH
    maximumHeight: _contentH

    onAboutToShow: MacOSPanel.beginShow(placementFraction, _h)
    onShown: MacOSPanel.finishShow(placementFraction, _h)

    MacOSWindow.enabled: true
    MacOSWindow.cornerRadius: cornerRadius
    MacOSWindow.blurEnabled: blurEnabled
    MacOSWindow.material: Config.windowMaterial === "liquid_glass" ? "liquidGlass" : "hud"
    MacOSWindow.appearance: Theme.isDark ? "dark" : "light"
    MacOSWindow.borderColor: Theme.mainWindowBorder
    MacOSWindow.borderWidth: Config.borderWidth

    MacOSPanel.enabled: true
    MacOSPanel.windowLevel: MacOSPanel.Status
    MacOSPanel.onResignKey: Nav.closeWindow()
}
