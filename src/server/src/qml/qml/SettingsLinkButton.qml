import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    implicitWidth: 200
    implicitHeight: 36
    radius: 8
    color: hoverHandler.hovered ? Theme.listItemHoverBg : Theme.secondaryBackground
    border.color: Theme.inputBorder
    border.width: 1

    property string icon: ""
    property string label: ""
    signal clicked()

    Row {
        id: innerRow
        anchors.centerIn: parent
        spacing: 8

        ViciImage {
            source: Img.builtin(root.icon)
            width: 16; height: 16
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: root.label
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    HoverHandler { id: hoverHandler }
    TapHandler {
        onTapped: root.clicked()
    }
}
