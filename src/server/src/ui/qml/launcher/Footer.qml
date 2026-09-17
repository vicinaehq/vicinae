pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

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
                visible: !Launcher.toastActive
                clickable: Launcher.atRoot
                availableWidth: parent.width
                anchors.verticalCenter: parent.verticalCenter
                onClicked: Launcher.openFooterMenu()
            }

            FooterToast {
                visible: Launcher.toastActive
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
            }
        }

        FooterButton {
            id: primaryButton
            visible: Launcher.actionPanel.primaryActionTitle !== ""
            Layout.alignment: Qt.AlignVCenter
            label: Launcher.actionPanel.primaryActionTitle
            shortcutTokens: Launcher.actionPanel.primaryActionShortcutTokens
            highlighted: true
            onClicked: Launcher.actionPanel.executePrimaryAction()
        }

        Rectangle {
            visible: primaryButton.visible && actionsButton.visible
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: 1
            Layout.preferredHeight: 12
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
            visible: Launcher.actionPanel.hasMultipleActions
            Layout.alignment: Qt.AlignVCenter
            label: qsTr("Actions")
            shortcutTokens: Keybinds.toggleActionPanelTokens
            highlighted: Launcher.actionPanel.open
            backgrounded: Launcher.actionPanel.open
            onClicked: Launcher.actionPanel.toggle(true)
        }
    }
}
