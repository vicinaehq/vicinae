import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root
    required property var host

    function moveUp() { listView.moveUp() }
    function moveDown() { listView.moveDown() }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 41
            Layout.leftMargin: 15
            Layout.rightMargin: 15
            spacing: 8

            Text {
                text: root.host.itemCountText
                color: Theme.foreground
                font.pointSize: Theme.regularFontSize
            }

            Item { Layout.fillWidth: true }

            Text {
                text: root.host.clipboardStatusText
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
            }

            Item {
                visible: root.host.clipboardStatusIcon !== ""
                width: 25
                height: 25
                opacity: statusMouseArea.containsMouse ? 1.0 : 0.6

                Image {
                    anchors.fill: parent
                    source: root.host.clipboardStatusIcon
                    sourceSize.width: 25
                    sourceSize.height: 25
                    asynchronous: true
                }

                MouseArea {
                    id: statusMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: root.host.canToggleMonitoring ? Qt.PointingHandCursor : Qt.ArrowCursor
                    enabled: root.host.canToggleMonitoring
                    onClicked: root.host.toggleMonitoring()
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: 1
            color: Theme.divider
        }

        GenericListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true

            listModel: root.host.listModel
            model: root.host.listModel
            autoWireModel: true
            selectFirstOnReset: root.host.listModel.selectFirstOnReset
            detailComponent: detailPanel
            detailVisible: root.host.hasDetail

            delegate: Loader {
                id: delegateLoader
                width: ListView.view.width

                required property int index
                required property bool isSection
                required property bool isSelectable
                required property string sectionName
                required property string title
                required property string subtitle
                required property string iconSource
                required property var itemAccessory
                required property bool isPinned

                sourceComponent: isSection ? sectionComponent : itemComponent

                Component {
                    id: sectionComponent
                    Item { width: 1; height: 0 }
                }

                Component {
                    id: itemComponent
                    SelectableDelegate {
                        id: itemDelegate
                        width: delegateLoader.width
                        height: 50
                        selected: listView.currentIndex === delegateLoader.index
                        onClicked: listView.currentIndex = delegateLoader.index
                        onDoubleClicked: listView.itemActivated(delegateLoader.index)

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 16
                            anchors.rightMargin: 16
                            spacing: 10

                            ViciImage {
                                visible: delegateLoader.iconSource !== ""
                                source: delegateLoader.iconSource
                                Layout.preferredWidth: 25
                                Layout.preferredHeight: 25
                                Layout.alignment: Qt.AlignVCenter
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2

                                Text {
                                    text: delegateLoader.title
                                    color: itemDelegate.selected
                                           ? Theme.listItemSelectionFg : Theme.foreground
                                    font.pointSize: Theme.regularFontSize
                                    elide: Text.ElideRight
                                    maximumLineCount: 1
                                    Layout.fillWidth: true
                                }

                                RowLayout {
                                    spacing: 5
                                    Image {
                                        visible: delegateLoader.isPinned
                                        source: "image://vicinae/builtin:pin?fg=" + Theme.red
                                        sourceSize.width: 14
                                        sourceSize.height: 14
                                        Layout.preferredWidth: 14
                                        Layout.preferredHeight: 14
                                    }
                                    Text {
                                        text: delegateLoader.subtitle
                                        color: Theme.textMuted
                                        font.pointSize: Theme.smallerFontSize
                                        elide: Text.ElideRight
                                        maximumLineCount: 1
                                        Layout.fillWidth: true
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: detailPanel

        DetailPanel {
            metadata: [
                {label: "Mime", value: root.host.detailMimeType,
                 icon: root.host.detailEncryptionIcon},
                {label: "Size", value: root.host.detailSize},
                {label: "Copied at", value: root.host.detailCopiedAt},
                {label: "MD5", value: root.host.detailMd5}
            ]

            Loader {
                anchors.fill: parent
                active: root.host.hasDetailError
                visible: active
                sourceComponent: EmptyView {
                    title: root.host.detailErrorTitle
                    description: root.host.detailErrorDescription
                    icon: "image://vicinae/builtin:key?fg=" + Theme.red
                }
            }

            Loader {
                anchors.fill: parent
                active: !root.host.hasDetailError && root.host.detailImageSource !== ""
                visible: active
                sourceComponent: Item {
                    ViciImage {
                        anchors.fill: parent
                        anchors.margins: 10
                        source: root.host.detailImageSource
                        fillMode: Image.PreserveAspectFit
                        sourceSize: Qt.size(width, height)
                        cache: false
                    }
                }
            }

            Loader {
                anchors.fill: parent
                active: !root.host.hasDetailError
                        && root.host.detailImageSource === ""
                        && root.host.detailTextContent !== ""
                visible: active
                sourceComponent: TextViewer {
                    text: root.host.detailTextContent
                    monospace: true
                }
            }

            Loader {
                anchors.fill: parent
                active: !root.host.hasDetailError
                        && root.host.detailImageSource === ""
                        && root.host.detailTextContent === ""
                visible: active
                sourceComponent: EmptyView {
                    title: root.host.detailMimeType
                    description: "Preview not available for this content type"
                }
            }
        }
    }
}
