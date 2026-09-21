pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

LauncherWindow {
    id: root
    appearance: LauncherAppearanceMacOS {}
    searchBarComponent: SearchBarMacOS {
        commandView: root.commandView
    }
    statusBarComponent: LauncherStatusBarMacOS {}
    contentEffect: ScrollFadeMacOS {
        topInset: root.headerOverlap
        bottomInset: root.statusBarOverlap
    }
    nativeChrome: true
    color: "transparent"
    shadowPadding: 0
    flags: Qt.Tool | Qt.FramelessWindowHint
    autoPlaceOnShow: false

    height: _contentH
    minimumHeight: _contentH
    maximumHeight: _contentH

    onAboutToShow: Launcher.prepareShow()
    onShown: Launcher.finalizeShow()

    MacOSWindow.enabled: true
    MacOSWindow.cornerRadius: cornerRadius
    MacOSWindow.blurEnabled: blurEnabled
    MacOSWindow.material: Config.windowMaterial === "liquid_glass" ? "liquidGlass" : "underWindowBackground"
    MacOSWindow.appearance: Theme.isDark ? "dark" : "light"
    MacOSWindow.borderColor: Config.withAlpha(Theme.foreground, Theme.isDark ? 0.16 : 0.12)
    MacOSWindow.borderWidth: Config.borderWidth

    MacOSPanel.enabled: true
    MacOSPanel.windowLevel: Launcher.filePicking ? MacOSPanel.Floating : MacOSPanel.Status
    MacOSPanel.onResignKey: if (!Launcher.filePicking)
        Launcher.nav.closeWindow()
}
