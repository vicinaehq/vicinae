import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Flickable {
    id: root
    contentWidth: width
    contentHeight: outer.implicitHeight
    clip: true
    boundsBehavior: Flickable.StopAtBounds

    readonly property var model: settings.generalModel

    ViciWheelHandler {
        target: root
    }

    ScrollBar.vertical: ViciScrollBar {
        policy: root.contentHeight > root.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
    }

    ColumnLayout {
        id: outer
        anchors.horizontalCenter: parent.horizontalCenter
        width: Math.min(root.width - 48, 720)
        spacing: 0

        SettingsSectionLabel {
            text: qsTr("Behavior")
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            SettingsRow {
                visible: Platform.supports("globalShortcuts")
                label: qsTr("Launcher hotkey")
                description: qsTr("Global shortcut to toggle the Vicinae launcher.")
                ShortcutField {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    bordered: false
                    shortcutId: GlobalShortcuts.toggleId
                    shortcut: root.model.toggleShortcut
                    onAccepted: shortcut => root.model.toggleShortcut = shortcut
                    onCleared: root.model.toggleShortcut = ""
                }
            }

            SettingsRow {
                label: qsTr("Close on focus loss")
                SettingsToggle {
                    checked: root.model.closeOnFocusLoss
                    onToggled: checked => root.model.closeOnFocusLoss = checked
                }
            }

            SettingsRow {
                label: qsTr("Close on Escape")
                description: qsTr("Pressing Escape closes the launcher instead of navigating one view back.")
                SettingsToggle {
                    checked: root.model.closeOnEscape
                    onToggled: checked => root.model.closeOnEscape = checked
                }
            }

            SettingsRow {
                label: qsTr("Pop to root on close")
                description: qsTr("Reset the navigation state when the launcher window is closed.")
                showSeparator: false
                SettingsToggle {
                    checked: root.model.popToRootOnClose
                    onToggled: root.model.popToRootOnClose = checked
                }
            }
        }

        SettingsSectionLabel {
            text: qsTr("Language")
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            SettingsRow {
                label: qsTr("Language")
                description: qsTr("Requires restarting Vicinae to take effect.")
                showSeparator: false
                SearchableDropdown {
                    width: parent.width
                    items: root.model.languageItems
                    currentItem: root.model.currentLanguage
                    onActivated: item => root.model.selectLanguage(item.id)
                }
            }
        }

        SettingsSectionLabel {
            text: qsTr("Privacy")
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            SettingsRow {
                label: qsTr("Basic usage statistics")
                description: qsTr("Send basic system and vicinae installation information on startup to help improve Vicinae.")
                showSeparator: false
                SettingsToggle {
                    checked: root.model.telemetrySystemInfo
                    onToggled: root.model.telemetrySystemInfo = checked
                }
            }
        }

        Item {
            Layout.preferredHeight: 24
        }
    }
}
