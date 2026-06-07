import QtQuick
import QtQuick.Layouts

Item {
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 4

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            FooterNavStatus {
                visible: !launcher.toastActive
                clickable: launcher.footerStatusClickable
                rootMode: launcher.atRoot
                availableWidth: parent.width
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
            highlighted: true
            onClicked: launcher.handleReturn()
        }

        Rectangle {
            visible: primaryButton.visible && actionsButton.visible
            Layout.alignment: Qt.AlignVCenter
            width: 1
            height: 12
            opacity: primaryButton.hovered || actionsButton.hovered || actionsButton.backgrounded ? 0 : 0.35
            color: Config.withAlpha(Theme.textMuted, Config.windowOpacity)

            Behavior on opacity {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutCubic
                }
            }
        }

        FooterButton {
            id: actionsButton
            visible: actionPanel.hasMultipleActions
            Layout.alignment: Qt.AlignVCenter
            label: "Actions"
            shortcutTokens: Keybinds.toggleActionPanelTokens
            highlighted: actionPanel.open
            backgrounded: actionPanel.open
            onClicked: actionPanel.toggle()
        }
    }
}
