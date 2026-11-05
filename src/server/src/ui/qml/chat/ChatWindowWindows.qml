import QtQuick
import Vicinae

ChatWindow {
    id: root
    translucentSidebar: false
    frostedHeader: false
    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint
    titlebarRightInset: WindowsWindow.captionButtonsRect.width
    WindowsWindow.enabled: true
    WindowsWindow.decorated: true
    WindowsWindow.titlebarHeight: root.headerHeight
    WindowsWindow.titlebarControls: root.titlebarControls
    WindowsWindow.appearance: Theme.isDark ? "dark" : "light"
    WindowsWindow.captionColor: Qt.tint(Theme.background, Config.withAlpha(Theme.foreground, 0.025))
    WindowsWindow.captionTitleVisible: false
}
