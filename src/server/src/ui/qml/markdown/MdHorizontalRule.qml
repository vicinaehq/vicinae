pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root

    property var blockData: ({})
    readonly property bool selected: selection.hasSelection

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

    DocumentSelection {
        id: selection
        selectedText: "---"
    }
}
