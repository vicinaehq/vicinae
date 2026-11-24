import QtQuick
import Vicinae

NotesWindow {
    id: root
    supportsKeepOnTop: true
    translucentBackground: false
    flags: (WindowsWindow.extendedTitlebarSupported ? Qt.Window | Qt.FramelessWindowHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint : Qt.Window | Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint) | root.stayOnTopFlag
    titlebarRightInset: WindowsWindow.captionButtonsRect.width
    WindowsWindow.enabled: true
    WindowsWindow.decorated: true
    WindowsWindow.titlebarHeight: root.headerHeight
    WindowsWindow.titlebarControls: root.titlebarControls
    WindowsWindow.appearance: Theme.isDark ? "dark" : "light"
    WindowsWindow.captionColor: Theme.background
    WindowsWindow.captionTitleVisible: false
}
