import QtQuick
import QtQuick.Layouts

Item {
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 8

        // Left side: Navigation status OR Toast
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            FooterNavStatus {
                visible: !launcher.toastActive
                anchors.verticalCenter: parent.verticalCenter
            }

            FooterToast {
                visible: launcher.toastActive
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        // Primary action label + shortcut badge
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

            ShortcutBadge {
                text: launcher.hasCommandView ? launcher.commandActionShortcut : "Enter"
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        // Actions button + Ctrl+B badge
        Item {
            visible: launcher.hasMultipleActions
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

                ShortcutBadge {
                    text: "Ctrl+B"
                    anchors.verticalCenter: parent.verticalCenter
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
    }
}
