import QtQuick

Item {
    id: root

    readonly property var _win: Window.window
    readonly property bool _popupWindow: _win !== null && (_win.flags & Qt.Popup) === Qt.Popup

    WindowsWindow.enabled: root._popupWindow
    WindowsWindow.blurEnabled: Config.blurEnabled
    WindowsWindow.appearance: Theme.isDark ? "dark" : "light"
    WindowsWindow.borderColor: Theme.divider
}
