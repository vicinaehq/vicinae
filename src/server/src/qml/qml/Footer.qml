import QtQuick
import QtQuick.Layouts

Item {
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 8

        // Actions button + Ctrl+B badge
        Item {
            visible: launcher.hasActions
            Layout.alignment: Qt.AlignVCenter
            implicitWidth: actionsRow.implicitWidth
            implicitHeight: actionsRow.implicitHeight

            Row {
                id: actionsRow
                spacing: 6

                Text {
                    text: "Actions"
                    color: actionsMouseArea.containsMouse ? Theme.foreground : Theme.textMuted
                    font.pointSize: Theme.smallerFontSize
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    width: actionsShortcutLabel.implicitWidth + 10
                    height: actionsShortcutLabel.implicitHeight + 4
                    radius: 4
                    color: Theme.divider
                    anchors.verticalCenter: parent.verticalCenter

                    Text {
                        id: actionsShortcutLabel
                        text: "Ctrl+B"
                        color: Theme.textMuted
                        font.pointSize: Theme.smallerFontSize - 1
                        anchors.centerIn: parent
                    }
                }
            }

            MouseArea {
                id: actionsMouseArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: launcher.toggleActionPanel()
            }
        }

        Item { Layout.fillWidth: true }

        // Primary action label + Enter badge
        Row {
            visible: {
                if (launcher.hasCommandView) return launcher.commandActionTitle !== ""
                return searchModel.primaryActionTitle !== ""
            }
            spacing: 6
            Layout.alignment: Qt.AlignVCenter

            Text {
                text: launcher.hasCommandView ? launcher.commandActionTitle : searchModel.primaryActionTitle
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                anchors.verticalCenter: parent.verticalCenter
            }

            Rectangle {
                width: enterLabel.implicitWidth + 10
                height: enterLabel.implicitHeight + 4
                radius: 4
                color: Theme.divider
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    id: enterLabel
                    text: "Enter"
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize - 1
                    anchors.centerIn: parent
                }
            }
        }
    }
}
