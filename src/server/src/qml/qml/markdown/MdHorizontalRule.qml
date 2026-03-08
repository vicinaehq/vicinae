import QtQuick
import Vicinae

Item {
    id: root

    property var blockData: ({})
    property var mdModel: null
    property int blockIndex: -1
    property var selectionController: null
    property bool selected: false

    width: parent?.width ?? 0
    implicitHeight: 17

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        height: 1
        color: Theme.divider
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.textSelectionBg
        opacity: 0.4
        visible: root.selected
    }

    onSelectionControllerChanged: if (selectionController) selectionController.registerSelectable(root, blockIndex * 10000, false)
    Component.onDestruction: if (selectionController) selectionController.unregisterSelectable(root)
}
