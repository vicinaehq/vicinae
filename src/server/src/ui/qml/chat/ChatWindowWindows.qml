import QtQuick
import Vicinae

ChatWindow {
    translucentSidebar: WindowsWindow.acrylicSupported
    WindowsWindow.enabled: true
    WindowsWindow.blurEnabled: true
    WindowsWindow.appearance: Theme.isDark ? "dark" : "light"
}
