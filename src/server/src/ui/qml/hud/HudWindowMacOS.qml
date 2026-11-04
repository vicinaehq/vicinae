pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

HudWindow {
    id: root

    readonly property int bottomMargin: dictating ? 16 : 96

    flags: Qt.Tool | Qt.FramelessWindowHint | Qt.WindowDoesNotAcceptFocus

    pillColor: dictating ? Config.withAlpha(Theme.background, 0.98) : "transparent"
    pillBorderColor: dictating ? Config.withAlpha(Theme.foreground, Theme.isDark ? 0.12 : 0.18) : "transparent"
    pillBorderWidth: dictating ? 1 : 0

    MacOSWindow.enabled: true
    MacOSWindow.blurEnabled: !dictating
    MacOSWindow.material: "liquidGlass"
    MacOSWindow.appearance: Theme.isDark ? "dark" : "light"
    MacOSWindow.cornerRadius: height / 2
    MacOSWindow.borderColor: Theme.mainWindowBorder
    MacOSWindow.borderWidth: dictating ? 0 : Config.borderWidth

    MacOSPanel.enabled: true
    MacOSPanel.windowLevel: MacOSPanel.Status

    function reposition() {
        if (visible)
            Qt.callLater(() => MacOSPanel.placeBottomCenter(bottomMargin));
    }

    onShown: reposition()
    onWidthChanged: reposition()
    onHeightChanged: reposition()
    onBottomMarginChanged: reposition()
}
