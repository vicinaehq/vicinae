pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

DropArea {
    id: root
    required property AttachmentModel attachmentModel
    onEntered: drag => {
        drag.accepted = root.attachmentModel !== null && drag.hasUrls && root.attachmentModel.acceptsUrls(drag.urls);
    }
    onDropped: drop => {
        if (root.attachmentModel !== null && drop.hasUrls && root.attachmentModel.acceptsUrls(drop.urls)) {
            root.attachmentModel.addUrls(drop.urls);
            drop.acceptProposedAction();
        }
    }
    Rectangle {
        anchors.fill: parent
        visible: root.containsDrag
        radius: 12
        color: Config.withAlpha(Theme.background, 0.95)
        border.color: Theme.accent
        border.width: 2
        Text {
            anchors.centerIn: parent
            text: qsTr("Drop images or text files")
            font.family: Theme.fontFamily
            font.pointSize: Theme.regularFontSize
            color: Theme.foreground
        }
    }
}
