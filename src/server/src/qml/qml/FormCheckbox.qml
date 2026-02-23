import QtQuick
import QtQuick.Layouts

Item {
    id: root
    implicitHeight: 28
    Layout.fillWidth: true
    activeFocusOnTab: !readOnly

    property bool checked: false
    property string label: ""
    property bool readOnly: false

    signal toggled()

    function toggle() {
        if (root.readOnly) return
        root.checked = !root.checked
        root.toggled()
    }

    Keys.onSpacePressed: toggle()
    Keys.onReturnPressed: (event) => {
        if (event.modifiers !== Qt.NoModifier && typeof launcher !== "undefined") {
            event.accepted = launcher.forwardKey(event.key, event.modifiers)
        } else {
            toggle()
        }
    }
    Keys.onPressed: (event) => {
        if (event.modifiers !== Qt.NoModifier && event.modifiers !== Qt.ShiftModifier
            && event.key !== Qt.Key_Shift && event.key !== Qt.Key_Control
            && event.key !== Qt.Key_Alt && event.key !== Qt.Key_Meta
            && typeof launcher !== "undefined") {
            event.accepted = launcher.forwardKey(event.key, event.modifiers)
        }
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: root.readOnly ? Qt.ArrowCursor : Qt.PointingHandCursor
        onClicked: root.toggle()
    }

    RowLayout {
        anchors.fill: parent
        spacing: 8
        opacity: root.readOnly ? 0.5 : 1.0

        Rectangle {
            id: box
            width: 16
            height: 16
            radius: 4
            Layout.alignment: Qt.AlignVCenter
            color: root.checked ? Theme.accent : "transparent"
            border.color: root.activeFocus ? Theme.inputBorderFocus
                          : root.checked ? Theme.accent : Theme.inputBorder
            border.width: 1

            Text {
                anchors.centerIn: parent
                text: "\u2713"
                color: "#ffffff"
                font.pixelSize: 11
                font.bold: true
                visible: root.checked
            }
        }

        Text {
            visible: root.label !== ""
            text: root.label
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize
            Layout.fillWidth: true
        }
    }
}
