LauncherWindow {
    nativeChrome: true
    color: "transparent"
    shadowPadding: 0
    flags: Qt.Dialog | Qt.FramelessWindowHint

    MacOSWindow.enabled: true
    MacOSWindow.cornerRadius: cornerRadius
    MacOSWindow.blurEnabled: blurEnabled
    MacOSWindow.material: "hud"
    MacOSWindow.borderColor: Theme.mainWindowBorder
    MacOSWindow.borderWidth: Config.borderWidth

    MacOSPanel.enabled: true
    MacOSPanel.onResignKey: Nav.closeWindow()
}
