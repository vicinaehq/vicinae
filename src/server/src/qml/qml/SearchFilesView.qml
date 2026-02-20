import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root
    required property var host

    function moveUp() { listView.moveUp() }
    function moveDown() { listView.moveDown() }

    GenericListView {
        id: listView
        anchors.fill: parent

        listModel: root.host.listModel
        model: root.host.listModel
        autoWireModel: true
        detailComponent: detailPanel
        detailVisible: root.host.hasDetail
        detailRatio: 0.55
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

            // Image preview
            Loader {
                anchors.fill: parent
                active: root.host.detailImageSource !== ""
                visible: active
                sourceComponent: Item {
                    ViciImage {
                        anchors.fill: parent
                        anchors.margins: 10
                        source: root.host.detailImageSource
                        fillMode: Image.PreserveAspectFit
                        sourceSize.width: width
                        sourceSize.height: height
                    }
                }
            }

            // Text preview
            Loader {
                anchors.fill: parent
                active: root.host.detailImageSource === ""
                        && root.host.detailTextContent !== ""
                visible: active
                sourceComponent: ScrollView {
                    clip: true
                    Text {
                        width: parent.width
                        text: root.host.detailTextContent
                        color: Theme.foreground
                        font.pointSize: Theme.smallerFontSize
                        font.family: "monospace"
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        padding: 12
                    }
                }
            }

            // Fallback — no preview
            Loader {
                anchors.fill: parent
                active: root.host.detailImageSource === ""
                        && root.host.detailTextContent === ""
                visible: active
                sourceComponent: EmptyView {
                    title: root.host.detailMimeType
                    description: "Preview not available for this file type"
                }
            }
        }
    }
}
