import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Flickable {
    id: root
    contentWidth: width
    contentHeight: col.implicitHeight
    clip: true
    boundsBehavior: Flickable.StopAtBounds

    readonly property var model: settings.generalModel

    ScrollBar.vertical: ViciScrollBar {
        policy: root.contentHeight > root.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
    }

    ColumnLayout {
        id: col
        width: parent.width
        spacing: 0

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
            FormTextInput {
                width: parent.width
                text: root.model.fontSize
                placeholder: "e.g. 11"
                onAccepted: root.model.fontSize = text
                onEditingChanged: {
                    if (!editing)
                        root.model.fontSize = text;
                }
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
            showSeparator: false
            SettingsToggle {
                checked: root.model.popToRootOnClose
                onToggled: root.model.popToRootOnClose = checked
            }
        }
    }
}
