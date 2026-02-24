import QtQuick

Item {
    id: root

    required property string label
    required property string shortcut
    property bool highlighted: false

    signal clicked()

    implicitWidth: row.implicitWidth
    implicitHeight: row.implicitHeight

    Row {
        id: row
        spacing: 6

        Text {
            text: root.label
            color: mouseArea.containsMouse || root.highlighted ? Theme.foreground : Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            anchors.verticalCenter: parent.verticalCenter
        }

        ShortcutBadge {
            text: root.shortcut
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.clicked()
    }
}
