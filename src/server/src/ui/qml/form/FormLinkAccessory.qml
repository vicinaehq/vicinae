pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    readonly property ExtensionViewHost host: Launcher.commandViewHost as ExtensionViewHost
    implicitWidth: linkText.implicitWidth + 20
    implicitHeight: linkText.implicitHeight

    Text {
        id: linkText
        anchors.centerIn: parent
        text: root.host ? root.host.linkAccessoryText : ""
        color: Theme.linkColor
        font.pointSize: Theme.smallerFontSize

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                if (root.host)
                    Qt.openUrlExternally(root.host.linkAccessoryHref);
            }
        }
    }
}
