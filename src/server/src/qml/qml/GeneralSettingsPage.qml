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

    ScrollBar.vertical: ViciScrollBar {
        policy: root.contentHeight > root.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
    }

    ColumnLayout {
        id: outer
        anchors.horizontalCenter: parent.horizontalCenter
        width: Math.min(root.width - 48, 720)
        spacing: 0

        SettingsSectionLabel {
            text: "Appearance"
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            SettingsRow {
                label: "Theme"
                SearchableDropdown {
                    width: parent.width
                    items: root.model.themeItems
                    currentItem: root.model.currentTheme
                    onActivated: item => root.model.selectTheme(item.id)
                }
            }

            SettingsRow {
                label: "Font"
                SearchableDropdown {
                    width: parent.width
                    items: root.model.fontItems
                    currentItem: root.model.currentFont
                    onActivated: item => root.model.selectFont(item.id)
                }
            }

            SettingsRow {
                label: "Font size"
                description: "The base point size used to compute font sizes. Fractional values are accepted. Recommended range is [10.0;12.0]."
                showSeparator: false
                FormTextInput {
                    width: parent.width
                    releaseFocusOnAccept: true
                    text: root.model.fontSize
                    placeholder: "e.g. 11"
                    onAccepted: root.model.fontSize = text
                    onEditingChanged: {
                        if (!editing)
                            root.model.fontSize = text;
                    }
                }
            }
        }

        SettingsSectionLabel {
            text: "Behavior"
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            SettingsRow {
                visible: settings.globalShortcutsSupported
                label: "Launcher hotkey"
                description: "Global shortcut to toggle the Vicinae launcher."
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
                label: "Close on focus loss"
                SettingsToggle {
                    checked: root.model.closeOnFocusLoss
                    onToggled: root.model.closeOnFocusLoss = checked
                }
            }

            SettingsRow {
                label: "Pop to root on close"
                description: "Reset the navigation state when the launcher window is closed."
                SettingsToggle {
                    checked: root.model.popToRootOnClose
                    onToggled: root.model.popToRootOnClose = checked
                }
            }

            SettingsRow {
                label: "Compact mode"
                description: "Show only the search bar at root; expand when a query is entered."
                showSeparator: false
                SettingsToggle {
                    checked: root.model.compactMode
                    onToggled: root.model.compactMode = checked
                }
            }
        }

        SettingsSectionLabel {
            text: "Privacy"
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            SettingsRow {
                label: "Basic usage statistics"
                description: "Send basic system and vicinae installation information on startup to help improve Vicinae."
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
