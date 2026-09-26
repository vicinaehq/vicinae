pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae.Documents

Rectangle {
    id: root
    required property string title
    required property string description
    property real progress: 1
    readonly property DocumentStyle style: root.DocumentScope.style
    implicitHeight: contents.implicitHeight + 32
    radius: 10
    color: root.style.secondaryBackground
    border.color: root.style.divider

    ColumnLayout {
        id: contents
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 16
        spacing: 12

        DocumentText {
            Layout.fillWidth: true
            text: root.title
            font.weight: Font.DemiBold
        }
        DocumentText {
            Layout.fillWidth: true
            text: root.description
        }
        Rectangle {
            id: progressItem
            Layout.fillWidth: true
            implicitHeight: 40
            radius: 6
            color: selection.hasSelection ? root.style.textSelectionBg : "transparent"

            DocumentSelection {
                id: selection
                selectedText: qsTr("Build progress: %1%").arg(Math.round(root.progress * 100))
            }

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 12
                ProgressBar {
                    Layout.fillWidth: true
                    value: root.progress
                }
                Label {
                    text: qsTr("%1%").arg(Math.round(root.progress * 100))
                    color: selection.hasSelection ? root.style.textSelectionFg : root.style.textMuted
                }
            }
        }
    }
}
