import QtQuick

Item {
    id: root
    implicitWidth: 36
    implicitHeight: 20
    anchors.right: parent ? parent.right : undefined
    activeFocusOnTab: true

    property bool checked: false
    signal toggled(bool checked)

    Rectangle {
        anchors.fill: parent
        radius: 10
        color: root.checked ? Theme.accent : Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.2)
        border.width: root.activeFocus ? 1 : 0
        border.color: Theme.inputBorderFocus
        Behavior on color {
            ColorAnimation {
                duration: 120
            }
        }

        Rectangle {
            width: 16
            height: 16
            radius: 8
            x: root.checked ? parent.width - width - 2 : 2
            anchors.verticalCenter: parent.verticalCenter
            color: "#ffffff"
            Behavior on x {
                NumberAnimation {
                    duration: 120
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: root.toggled(!root.checked)
    }

    Keys.onReturnPressed: root.toggled(!root.checked)
    Keys.onSpacePressed: root.toggled(!root.checked)
}
