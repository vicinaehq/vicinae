pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

// One model of a provider. The state slot has a fixed width so rows never shift between states.
ColumnLayout {
    id: root

    required property int index
    required property string modelId
    required property string name
    required property string description
    required property string meta
    required property var icon
    required property string status
    required property real progress

    signal download
    signal cancelDownload
    signal deleteRequested

    readonly property bool usable: status === "available" || status === "installed"

    Layout.fillWidth: true
    spacing: 0

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 1
        color: Theme.divider
        visible: root.index > 0
    }

    Item {
        Layout.fillWidth: true
        implicitHeight: row.implicitHeight + 24

        HoverHandler {
            id: rowHover
        }

        RowLayout {
            id: row
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 16
            anchors.rightMargin: 16
            spacing: 12

            ViciImage {
                source: root.icon ?? Img.icon(BuiltinIcon.ComputerChip).withFillColor(Theme.textMuted)
                Layout.preferredWidth: 22
                Layout.preferredHeight: 22
                Layout.alignment: Qt.AlignTop
                opacity: root.usable ? 1.0 : 0.4
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Text {
                    text: root.name
                    color: root.usable ? Theme.foreground : Theme.textMuted
                    font.pointSize: Theme.regularFontSize
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
                    visible: root.description !== ""
                    text: root.description
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize
                    wrapMode: Text.Wrap
                    maximumLineCount: 2
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
                    visible: root.meta !== ""
                    text: root.meta
                    color: Theme.textPlaceholder
                    font.pointSize: Theme.smallerFontSize - 1
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }

            Item {
                Layout.preferredWidth: 124
                Layout.minimumWidth: 124
                Layout.preferredHeight: 26
                Layout.alignment: Qt.AlignVCenter

                ViciButton {
                    visible: root.status === "absent"
                    anchors.fill: parent
                    variant: "ghost"
                    bordered: true
                    contentAlignment: Qt.AlignLeft
                    iconSource: Img.icon(BuiltinIcon.Download).withFillColor(Theme.foreground)
                    text: qsTr("Download")
                    onClicked: root.download()
                }

                ViciButton {
                    visible: root.status === "downloading"
                    anchors.fill: parent
                    variant: "ghost"
                    bordered: true
                    contentAlignment: Qt.AlignLeft
                    busy: !hovered
                    iconSource: hovered ? Img.icon(BuiltinIcon.Xmark).withFillColor(Theme.foreground) : undefined
                    text: hovered ? qsTr("Cancel") : (root.progress < 0 ? qsTr("Downloading") : qsTr("%1%").arg(Math.round(root.progress * 100)))
                    onClicked: root.cancelDownload()
                }

                ViciIconButton {
                    visible: root.status === "installed"
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    opacity: rowHover.hovered ? 1.0 : 0.0
                    iconSource: Img.icon(BuiltinIcon.Trash)
                    danger: true
                    tooltip: qsTr("Delete model file")
                    onClicked: root.deleteRequested()

                    Behavior on opacity {
                        NumberAnimation {
                            duration: 120
                        }
                    }
                }
            }
        }
    }
}
