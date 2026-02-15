import QtQuick
import QtQuick.Layouts

Item {
    id: root
    implicitHeight: 28
    Layout.fillWidth: true
    activeFocusOnTab: true

    property bool checked: false
    property string label: ""

    signal toggled()

    function toggle() {
        root.checked = !root.checked
        root.toggled()
    }

    Keys.onSpacePressed: toggle()
    Keys.onReturnPressed: (event) => {
        if (event.modifiers !== Qt.NoModifier) {
            event.accepted = launcher.forwardKey(event.key, event.modifiers)
        } else {
            toggle()
        }
    }
    Keys.onPressed: (event) => {
        if (event.modifiers !== Qt.NoModifier && event.modifiers !== Qt.ShiftModifier
            && event.key !== Qt.Key_Shift && event.key !== Qt.Key_Control
            && event.key !== Qt.Key_Alt && event.key !== Qt.Key_Meta) {
            event.accepted = launcher.forwardKey(event.key, event.modifiers)
        }
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: root.toggle()
    }

    RowLayout {
        anchors.fill: parent
        spacing: 8

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
