pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root

    property Item header: null
    property Item footer: null
    readonly property LauncherAppearance appearance: (root.Window.window as LauncherWindow)?.appearance ?? fallbackAppearance

    LauncherAppearance {
        id: fallbackAppearance
    }
}
