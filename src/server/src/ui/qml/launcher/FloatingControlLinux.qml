pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    required property Item anchorItem
    default property alias content: content.data
    x: anchorItem.x
    y: anchorItem.y
    width: anchorItem.width
    height: anchorItem.height
    z: 1
    clip: true
    visible: anchorItem.visible && !Launcher.compacted && !Launcher.hasOverlay && !Launcher.alertModel.visible

    Rectangle {
        anchors.fill: parent
        radius: root.height / 2
        color: Config.withAlpha(Theme.statusBarBackground, 0.78 * Config.windowOpacity)
        border.width: 1
        border.color: Config.withAlpha(Theme.foreground, Theme.isDark ? 0.1 : 0.08)
    }

    Item {
        id: content
        anchors.fill: parent
    }
}
