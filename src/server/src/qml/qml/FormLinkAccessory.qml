import QtQuick

Item {
    id: root
    implicitWidth: linkText.implicitWidth + 20
    implicitHeight: linkText.implicitHeight

    Text {
        id: linkText
        anchors.centerIn: parent
        text: launcher.commandViewHost ? launcher.commandViewHost.linkAccessoryText : ""
        color: Theme.accent
        font.pointSize: Theme.smallerFontSize

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                if (launcher.commandViewHost)
                    Qt.openUrlExternally(launcher.commandViewHost.linkAccessoryHref)
            }
        }
    }
}
