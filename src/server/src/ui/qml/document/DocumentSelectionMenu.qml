pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import Vicinae

Popup {
    id: selectionMenu
    required property DocumentController controller
    width: 160
    margins: 8
    topPadding: 6
    bottomPadding: 6
    leftPadding: 1
    rightPadding: 1

    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Item {
        Rectangle {
            anchors.fill: parent
            radius: Config.borderRounding
            color: Qt.rgba(Theme.statusBarBackground.r, Theme.statusBarBackground.g, Theme.statusBarBackground.b, 1)

            layer.enabled: true
            layer.effect: MultiEffect {
                autoPaddingEnabled: true
                shadowEnabled: true
                shadowBlur: 0.4
                shadowColor: Qt.rgba(0, 0, 0, 0.25)
                shadowVerticalOffset: 4
            }
        }

        Rectangle {
            anchors.fill: parent
            radius: Config.borderRounding
            color: Qt.rgba(Theme.statusBarBackground.r, Theme.statusBarBackground.g, Theme.statusBarBackground.b, 1)
            border.color: Config.withAlpha(Theme.mainWindowBorder, Config.windowOpacity)
            border.width: 1
        }
    }

    contentItem: ColumnLayout {
        spacing: 0

        ActionItemDelegate {
            Layout.fillWidth: true
            title: qsTr("Copy")
            iconSource: ""
            shortcutTokens: []
            isSubmenu: false
            isDanger: false
            opacity: selectionMenu.controller.hasSelection ? 1.0 : 0.5
            enabled: selectionMenu.controller.hasSelection

            onClicked: {
                selectionMenu.controller.copy();
                selectionMenu.close();
            }
        }

        ActionItemDelegate {
            Layout.fillWidth: true
            title: qsTr("Select All")
            iconSource: ""
            shortcutTokens: []
            isSubmenu: false
            isDanger: false

            onClicked: {
                selectionMenu.controller.selectAll();
                selectionMenu.close();
            }
        }
    }
}
