pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

LauncherWindow {
    nativeChrome: true
    cornerRadius: WindowsWindow.nativeCornerRadius
    flags: Qt.Tool | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    height: _contentH
    minimumHeight: _contentH
    maximumHeight: _contentH

    WindowsWindow.enabled: true
    WindowsWindow.blurEnabled: blurEnabled
    WindowsWindow.appearance: Theme.isDark ? "dark" : "light"
    WindowsWindow.borderColor: Theme.mainWindowBorder

    onActiveChanged: {
        if (!active && visible && !Launcher.filePicking)
            Launcher.nav.closeWindow();
    }
}
