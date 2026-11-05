pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

SettingsWindow {
    id: root
    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowMinimizeButtonHint
    titlebarRightInset: WindowsWindow.captionButtonsRect.width
    WindowsWindow.enabled: true
    WindowsWindow.decorated: true
    WindowsWindow.titlebarHeight: root.headerHeight
    WindowsWindow.titlebarControls: root.titlebarControls
    WindowsWindow.appearance: Theme.isDark ? "dark" : "light"
    WindowsWindow.captionColor: Theme.background
    WindowsWindow.captionTitleVisible: false
}
