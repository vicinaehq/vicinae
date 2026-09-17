pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    property bool vertical: false
    property color fallbackColor: Config.withAlpha(Theme.divider, Config.windowOpacity)
    readonly property LauncherAppearance appearance: (root.Window.window as LauncherWindow)?.appearance ?? fallbackAppearance
    implicitWidth: vertical ? 1 : -1
    implicitHeight: vertical ? -1 : 1

    Loader {
        id: renderer
        anchors.fill: parent
        sourceComponent: root.appearance.dividerComponent ?? defaultDivider
    }

    LauncherAppearance {
        id: fallbackAppearance
    }

    Component {
        id: defaultDivider
        ViciDivider {
            color: root.fallbackColor
        }
    }

    Binding {
        target: renderer.item
        property: "vertical"
        value: root.vertical
    }
}
