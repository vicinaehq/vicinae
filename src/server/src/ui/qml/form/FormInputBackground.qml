pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

SourceBlendRect {
    id: root

    property bool filled: false
    readonly property bool _enabled: opacity >= 1
    readonly property LauncherAppearance appearance: (root.Window.window as LauncherWindow)?.appearance ?? fallbackAppearance

    LauncherAppearance {
        id: fallbackAppearance
    }

    visible: filled
    radius: 8
    backgroundColor: _enabled ? appearance.delegateBackdrop : "transparent"
    color: Config.withAlpha(Theme.inputBackground, Config.surfaceOpacity)
}
