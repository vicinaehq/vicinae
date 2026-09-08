import QtQuick
import Vicinae

Item {
    id: root
    implicitWidth: linkText.implicitWidth + 20
    implicitHeight: linkText.implicitHeight

    Text {
        id: linkText
        anchors.centerIn: parent
        text: Launcher.commandViewHost ? Launcher.commandViewHost.linkAccessoryText : ""
        color: Theme.linkColor
        font.pointSize: Theme.smallerFontSize

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                if (Launcher.commandViewHost)
                    Qt.openUrlExternally(Launcher.commandViewHost.linkAccessoryHref);
            }
        }
    }
}
