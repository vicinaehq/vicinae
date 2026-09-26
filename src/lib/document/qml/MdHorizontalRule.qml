pragma ComponentBehavior: Bound
import QtQuick
import Vicinae.Documents

Item {
    id: root
    readonly property DocumentStyle style: root.DocumentScope.style

    property var blockData: ({})
    readonly property bool selected: selection.hasSelection

    width: parent?.width ?? 0
    implicitHeight: 17

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        height: 1
        color: root.style.divider
    }

    Rectangle {
        anchors.fill: parent
        color: root.style.textSelectionBg
        opacity: 0.4
        visible: root.selected
    }

    DocumentSelection {
        id: selection
        selectedText: "---"
    }
}
