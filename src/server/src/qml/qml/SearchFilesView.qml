import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root
    required property var host

    function moveUp() { listView.moveUp() }
    function moveDown() { listView.moveDown() }

    CommandListView {
        id: listView
        anchors.fill: parent

        cmdModel: root.host.listModel
        detailComponent: detailPanel
        detailVisible: root.host.hasDetail

    }

    Component {
        id: detailPanel

        DetailPanel {
            metadata: [
                {label: "Name", value: root.host.detailName},
                {label: "Where", value: root.host.detailPath},
                {label: "Type", value: root.host.detailMimeType},
                {label: "Last modified", value: root.host.detailLastModified}
            ]

            FilePreview {
                anchors.fill: parent
                imageSource: root.host.detailImageSource
                textContent: root.host.detailTextContent
                mimeType: root.host.detailMimeType
            }
        }
    }
}
