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

        FooterButton {
            id: primaryButton
            visible: {
                if (!launcher.isRootSearch) return actionPanel.primaryActionTitle !== ""
                return searchModel.primaryActionTitle !== ""
            }
            Layout.alignment: Qt.AlignVCenter
            label: !launcher.isRootSearch ? actionPanel.primaryActionTitle : searchModel.primaryActionTitle
            shortcut: !launcher.isRootSearch ? actionPanel.primaryActionShortcut : "↵"
            onClicked: launcher.handleReturn()
        }

        Rectangle {
            visible: primaryButton.visible && actionsButton.visible
            Layout.alignment: Qt.AlignVCenter
            width: 1
            height: 14
            color: Theme.divider
        }

        FooterButton {
            id: actionsButton
            visible: actionPanel.hasMultipleActions
            Layout.alignment: Qt.AlignVCenter
            label: "Actions"
            shortcut: Keybinds.toggleActionPanel
            highlighted: actionPanel.open
            onClicked: actionPanel.toggle()
        }
    }
}
