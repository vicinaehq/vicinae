import QtQuick
import Vicinae

Item {
    property var blockData: ({})
    property var mdModel: null
    property int blockIndex: -1

    width: parent?.width ?? 0
    implicitHeight: 17

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        height: 1
        color: Theme.divider
    }
}
