import QtQuick

BarPill {
    id: root

    tooltip: qsTr("Open Vicinae")
    horizontalPadding: 6

    onClicked: mouse => {
        if (mouse.button === Qt.RightButton)
            bar.openSettings();
        else
            bar.toggleLauncher();
    }

    Item {
        width: 20
        height: 20

        ViciImage {
            anchors.fill: parent
            source: Img.builtin("vicinae").withTemplateFill(Theme.foreground)
            sourceSize: Qt.size(20, 20)
        }

        Rectangle {
            visible: bar.hasUnreadNews
            width: 6
            height: 6
            radius: 3
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: -2
            anchors.rightMargin: -2
            color: Theme.accent
        }
    }
}
