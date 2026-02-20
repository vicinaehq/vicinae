import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    implicitHeight: 36
    radius: 8
    color: hoverHandler.hovered ? Theme.listItemHoverBg : Theme.secondaryBackground
    border.color: Theme.inputBorder
    border.width: 1

    property string icon: ""
    property string label: ""
    signal clicked()

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        spacing: 8

        ViciImage {
            source: Img.builtin(root.icon)
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
        }

        Text {
            text: root.label
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize
            Layout.fillWidth: true
        }
    }

    HoverHandler { id: hoverHandler }
    TapHandler {
        onTapped: root.clicked()
    }
}
