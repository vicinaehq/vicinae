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
            label: "Icon Theme"
            description: "The icon theme used for system icons (applications, mime types, folder icons...). Does not affect builtin Vicinae icons."
            SearchableDropdown {
                width: parent.width
                items: root.model.iconThemeItems
                currentItem: root.model.currentIconTheme
                onActivated: item => root.model.selectIconTheme(item.id)
            }
        }

        SettingsRow {
            label: "Window opacity"
            FormTextInput {
                width: parent.width
                text: root.model.windowOpacity
                placeholder: "e.g. 1.0"
                onAccepted: root.model.windowOpacity = text
                onEditingChanged: {
                    if (!editing)
                        root.model.windowOpacity = text;
                }
            }
        }

        SettingsRow {
            label: "Client-side decorations"
            description: "Let Vicinae draw its own rounded borders instead of relying on the windowing system."
            SettingsToggle {
                checked: root.model.clientSideDecorations
                onToggled: root.model.clientSideDecorations = checked
            }
        }

        SettingsRow {
            label: "Pop on backspace"
            description: "Pop back in navigation on backspace when no input is present."
            SettingsToggle {
                checked: root.model.popOnBackspace
                onToggled: root.model.popOnBackspace = checked
            }
        }

        SettingsRow {
            label: "Root file search"
            description: "Files are searched asynchronously, so if enabled you should expect a slight delay for file search results to show up."
            SettingsToggle {
                checked: root.model.searchFilesInRoot
                onToggled: root.model.searchFilesInRoot = checked
            }
        }

        SettingsRow {
            label: "IME handling"
            description: "Include IME Preedit strings as part of search queries."
            SettingsToggle {
                checked: root.model.considerPreedit
                onToggled: root.model.considerPreedit = checked
            }
        }

        SettingsRow {
            label: "Favicon Fetching"
            description: "The favicon provider used to load favicons where needed. Select 'None' to turn off favicon loading."
            SearchableDropdown {
                width: parent.width
                items: root.model.faviconServiceItems
                currentItem: root.model.currentFaviconService
                onActivated: item => root.model.selectFaviconService(item.id)
            }
        }

        SettingsRow {
            label: "Keybinding Scheme"
            description: "Default uses Vim-style Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Opt+B/F for navigation, plus Emacs editing in the search bar."
            showSeparator: false
            SearchableDropdown {
                width: parent.width
                items: root.model.keybindingSchemeItems
                currentItem: root.model.currentKeybindingScheme
                onActivated: item => root.model.selectKeybindingScheme(item.id)
            }
        }
    }
}
