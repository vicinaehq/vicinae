pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

SourceBlendRect {
    id: root

    property bool filled: false
    property bool opaque: false
    readonly property bool _enabled: opacity >= 1
    readonly property LauncherAppearance appearance: (root.Window.window as LauncherWindow)?.appearance ?? fallbackAppearance

    LauncherAppearance {
        id: fallbackAppearance
    }

    visible: filled
    radius: 8
    backgroundColor: !opaque && _enabled ? appearance.delegateBackdrop : "transparent"
    color: opaque ? Qt.tint(Theme.background, Config.withAlpha(Theme.inputBackground, Config.surfaceOpacity)) : Config.withAlpha(Theme.inputBackground, Config.surfaceOpacity)
}
