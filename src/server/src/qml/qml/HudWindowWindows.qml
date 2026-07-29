import QtQuick

HudWindow {
    id: root

    readonly property int bottomMargin: 96

    pillColor: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
    pillBorderColor: "transparent"
    pillBorderWidth: 0
    pillRadius: 0

    WindowsWindow.enabled: true
    WindowsWindow.blurEnabled: true
    WindowsWindow.appearance: Theme.isDark ? "dark" : "light"
    WindowsWindow.borderColor: Theme.mainWindowBorder

    function reposition() {
        if (!visible)
            return;
        const g = launcher.cursorScreenGeometry();
        x = g.x + (g.width - width) / 2;
        y = g.y + g.height - height - bottomMargin;
    }

    onShown: reposition()
    onWidthChanged: reposition()
}
