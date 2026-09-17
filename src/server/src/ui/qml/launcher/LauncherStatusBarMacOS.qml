pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

Item {
    id: root
    implicitHeight: 48

    Item {
        id: menuAnchor
        x: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        width: 34
        height: 34
        visible: Launcher.atRoot && !Launcher.toastActive
    }

    FloatingControlMacOS {
        anchorItem: menuAnchor
        title: qsTr("Vicinae menu")

        AbstractButton {
            id: menuButton
            anchors.fill: parent
            Accessible.name: qsTr("Vicinae menu")
            focusPolicy: Qt.NoFocus
            padding: 8
            hoverEnabled: true
            onClicked: Launcher.openFooterMenu()
            background: Rectangle {
                radius: height / 2
                color: Config.withAlpha(Theme.foreground, menuButton.down || Launcher.footerPanel.open ? 0.13 : menuButton.hovered ? 0.07 : 0)
            }
            contentItem: ViciImage {
                source: Img.icon(BuiltinIcon.Vicinae).withFillColor(Theme.foreground)
            }
        }
    }

    Item {
        id: statusAnchor
        x: menuAnchor.visible ? menuAnchor.x + menuAnchor.width + 8 : 10
        readonly property real availableWidth: Math.max(0, (actionAnchor.visible ? actionAnchor.x - 10 : root.width - 10) - x)
        width: Math.min(Math.ceil((Launcher.toastActive ? toast.naturalWidth : navStatus.naturalWidth) + 24), Math.floor(availableWidth))
        height: 34
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        visible: Launcher.toastActive || Launcher.navigationTitle !== "" || (!Launcher.atRoot && Launcher.navigationIcon.valid)
    }

    FloatingControlMacOS {
        anchorItem: statusAnchor
        title: Launcher.toastActive ? Launcher.toastTitle : Launcher.navigationTitle

        FooterNavStatus {
            id: navStatus
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 12
            anchors.verticalCenter: parent.verticalCenter
            availableWidth: width
            showIcon: !Launcher.atRoot
            textColor: Theme.foreground
            visible: !Launcher.toastActive
        }

        FooterToast {
            id: toast
            anchors.fill: parent
            anchors.leftMargin: 12
            anchors.rightMargin: 12
            constrained: true
            visible: Launcher.toastActive
        }
    }

    Item {
        id: actionAnchor
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        width: Math.min(actionRow.implicitWidth + 8, root.width * 0.65)
        height: 34
        visible: Launcher.actionPanel.primaryActionTitle !== "" || Launcher.actionPanel.hasMultipleActions
    }

    FloatingControlMacOS {
        anchorItem: actionAnchor
        title: qsTr("Vicinae actions")

        RowLayout {
            id: actionRow
            anchors.fill: parent
            anchors.margins: 4
            spacing: 0

            ActionButton {
                id: primaryButton
                Layout.fillWidth: true
                visible: Launcher.actionPanel.primaryActionTitle !== ""
                label: Launcher.actionPanel.primaryActionTitle
                tokens: Launcher.actionPanel.primaryActionShortcutTokens
                primary: true
                onClicked: Launcher.actionPanel.executePrimaryAction()
            }

            Rectangle {
                visible: primaryButton.visible && actionsButton.visible
                Layout.preferredWidth: 1
                Layout.preferredHeight: 12
                color: Config.withAlpha(Theme.foreground, 0.12)
            }

            ActionButton {
                id: actionsButton
                visible: Launcher.actionPanel.hasMultipleActions
                label: qsTr("Actions")
                tokens: Keybinds.toggleActionPanelTokens
                active: Launcher.actionPanel.open
                onClicked: Launcher.actionPanel.toggle(true)
            }
        }
    }

    component ActionButton: AbstractButton {
        id: button
        required property string label
        required property var tokens
        property bool primary: false
        property bool active: false

        implicitWidth: labelRow.implicitWidth + 16
        implicitHeight: 26
        leftPadding: 8
        rightPadding: 8
        focusPolicy: Qt.NoFocus
        hoverEnabled: true
        Accessible.name: label

        background: Rectangle {
            radius: height / 2
            color: Config.withAlpha(Theme.foreground, button.down || button.active ? 0.13 : button.hovered ? 0.08 : 0)
        }

        contentItem: RowLayout {
            id: labelRow
            spacing: 6

            Text {
                Layout.fillWidth: true
                text: button.label
                font.family: Theme.fontFamily
                font.pointSize: Theme.smallerFontSize
                font.weight: button.primary ? Font.Medium : Font.Normal
                color: Theme.foreground
                elide: Text.ElideRight
            }

            ShortcutBadge {
                tokens: button.tokens
                contentColor: Config.withAlpha(Theme.foreground, 0.85)
                keycapSize: 18
                surfaceColor: Config.withAlpha(Theme.foreground, 0.06)
                borderColor: Config.withAlpha(Theme.foreground, Theme.isDark ? 0.26 : 0.2)
            }
        }
    }
}
