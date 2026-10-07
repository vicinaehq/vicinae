pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    required property Item anchorItem
    property string title
    default property alias content: content.data
    x: anchorItem.x
    y: anchorItem.y
    width: anchorItem.width
    height: anchorItem.height
    z: 1
    visible: anchorItem.visible && !Launcher.compacted && !Launcher.hasOverlay && !Launcher.alertModel.visible

    GlassSurfaceMacOS {
        anchors.fill: parent
        radius: root.height / 2
    }

    Item {
        id: content
        anchors.fill: parent
    }
}
