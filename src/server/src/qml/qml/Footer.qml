import QtQuick
import QtQuick.Layouts

Item {
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 8

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
                width: parent.width
            }
        }

        Row {
            visible: {
                if (launcher.hasCommandView) return actionPanel.primaryActionTitle !== ""
                return searchModel.primaryActionTitle !== ""
            }
            spacing: 6
            Layout.alignment: Qt.AlignVCenter

            Text {
                text: launcher.hasCommandView ? actionPanel.primaryActionTitle : searchModel.primaryActionTitle
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                anchors.verticalCenter: parent.verticalCenter
            }

            ShortcutBadge {
                text: launcher.hasCommandView ? actionPanel.primaryActionShortcut : "Enter"
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Item {
            visible: actionPanel.hasMultipleActions
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
                onClicked: actionPanel.toggle()
            }
        }
    }
}
