LauncherWindow {
    nativeChrome: true
    color: "transparent"
    shadowPadding: 0
    flags: Qt.Tool | Qt.FramelessWindowHint
    autoPlaceOnShow: false

    height: _contentH
    minimumHeight: _contentH
    maximumHeight: _contentH

    onAboutToShow: launcher.prepareShow()
    onShown: launcher.finalizeShow()

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
