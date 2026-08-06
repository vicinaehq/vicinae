import QtQuick

HudWindow {
    id: root

    readonly property int bottomMargin: 96

    flags: Qt.Tool | Qt.FramelessWindowHint | Qt.WindowDoesNotAcceptFocus

    // The glass material provides the background, so the shared pill stays transparent.
    pillColor: "transparent"
    pillBorderColor: "transparent"
    pillBorderWidth: 0

    MacOSWindow.enabled: true
    MacOSWindow.blurEnabled: true
    MacOSWindow.material: "liquidGlass"
    MacOSWindow.appearance: Theme.isDark ? "dark" : "light"
    MacOSWindow.cornerRadius: height / 2
    MacOSWindow.borderColor: Theme.mainWindowBorder
    MacOSWindow.borderWidth: Config.borderWidth

    MacOSPanel.enabled: true
    MacOSPanel.windowLevel: MacOSPanel.Status

    function reposition() {
        if (visible)
            Qt.callLater(() => MacOSPanel.placeBottomCenter(bottomMargin));
    }

    onShown: reposition()
    onWidthChanged: reposition()
}
