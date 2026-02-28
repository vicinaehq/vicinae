import QtQuick
import QtQuick.Layouts

Item {
    id: root
    readonly property var model: settings.generalModel

    FormView {
        anchors.fill: parent
        maxContentWidth: 650

        FormField {
            label: "Theme"
            SearchableDropdown {
                items: root.model.themeItems
                currentItem: root.model.currentTheme
                onActivated: item => root.model.selectTheme(item.id)
            }
        }

        FormField {
            label: "Font"
            SearchableDropdown {
                items: root.model.fontItems
                currentItem: root.model.currentFont
                onActivated: item => root.model.selectFont(item.id)
            }
        }

        FormField {
            label: "Font size"
            info: "The base point size used to compute font sizes. Fractional values are accepted. Recommended range is [10.0;12.0]."
            FormTextInput {
                text: root.model.fontSize
                placeholder: "e.g. 11"
                onAccepted: root.model.fontSize = text
                onActiveFocusChanged: {
                    if (!activeFocus)
                        root.model.fontSize = text;
                }
            }
        }

        FormField {
            label: "Icon Theme"
            info: "The icon theme used for system icons (applications, mime types, folder icons...). Does not affect builtin Vicinae icons."
            SearchableDropdown {
                items: root.model.iconThemeItems
                currentItem: root.model.currentIconTheme
                onActivated: item => root.model.selectIconTheme(item.id)
            }
        }

        FormSeparator {}

        FormField {
            label: "Window opacity"
            FormTextInput {
                text: root.model.windowOpacity
                placeholder: "e.g. 1.0"
                onAccepted: root.model.windowOpacity = text
                onActiveFocusChanged: {
                    if (!activeFocus)
                        root.model.windowOpacity = text;
                }
            }
        }

        FormField {
            label: "CSD"
            info: "Let Vicinae draw its own rounded borders instead of relying on the windowing system."
            FormCheckbox {
                label: "Use client-side decorations"
                checked: root.model.clientSideDecorations
                onToggled: root.model.clientSideDecorations = checked
            }
        }

        FormField {
            label: "Pop on close"
            info: "Whether to reset the navigation state when the launcher window is closed."
            FormCheckbox {
                label: "Pop to root on window close"
                checked: root.model.popToRootOnClose
                onToggled: root.model.popToRootOnClose = checked
            }
        }

        FormField {
            label: "Pop on backspace"
            info: "Whether to pop back in navigation on backspace when no input is present"
            FormCheckbox {
                label: "Pop back on backspace"
                checked: root.model.popOnBackspace
                onToggled: root.model.popOnBackspace = checked
            }
        }

        FormField {
            label: "Focus handling"
            FormCheckbox {
                label: "Close on focus loss"
                checked: root.model.closeOnFocusLoss
                onToggled: root.model.closeOnFocusLoss = checked
            }
        }

        FormSeparator {}

        FormField {
            label: "Root file search"
            info: "Files are searched asynchronously, so if enabled you should expect a slight delay for file search results to show up."
            FormCheckbox {
                label: "Show files in root search"
                checked: root.model.searchFilesInRoot
                onToggled: root.model.searchFilesInRoot = checked
            }
        }

        FormField {
            label: "IME handling"
            info: "Whether to include IME Preedit strings as part of search queries."
            FormCheckbox {
                label: "Include Preedit strings in search"
                checked: root.model.considerPreedit
                onToggled: root.model.considerPreedit = checked
            }
        }

        FormSeparator {}

        FormField {
            label: "Favicon Fetching"
            info: "The favicon provider used to load favicons where needed. Select 'None' to turn off favicon loading."
            SearchableDropdown {
                items: root.model.faviconServiceItems
                currentItem: root.model.currentFaviconService
                onActivated: item => root.model.selectFaviconService(item.id)
            }
        }

        FormField {
            label: "Keybinding Scheme"
            info: "Default uses Vim-style Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Opt+B/F for navigation, plus Emacs editing in the search bar."
            SearchableDropdown {
                items: root.model.keybindingSchemeItems
                currentItem: root.model.currentKeybindingScheme
                onActivated: item => root.model.selectKeybindingScheme(item.id)
            }
        }
    }
}
