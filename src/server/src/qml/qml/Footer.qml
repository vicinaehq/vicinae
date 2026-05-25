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
                clickable: launcher.atRoot
                anchors.verticalCenter: parent.verticalCenter
                onClicked: launcher.openFooterMenu()
            }

            FooterToast {
                visible: launcher.toastActive
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
            }
        }

        FooterButton {
            id: primaryButton
            visible: actionPanel.primaryActionTitle !== ""
            Layout.alignment: Qt.AlignVCenter
            label: actionPanel.primaryActionTitle
            shortcutTokens: actionPanel.primaryActionShortcutTokens
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
            shortcutTokens: Keybinds.toggleActionPanelTokens
            highlighted: actionPanel.open
            onClicked: actionPanel.toggle()
        }
    }
}
