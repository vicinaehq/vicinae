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
            text: "Input & Navigation"
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            SettingsRow {
                label: "Pop on backspace"
                description: "Pop back in navigation on backspace when no input is present."
                SettingsToggle {
                    checked: root.model.popOnBackspace
                    onToggled: root.model.popOnBackspace = checked
                }
            }

            SettingsRow {
                label: "Activate on single click"
                description: "Activate items with a single click instead of requiring a double click."
                SettingsToggle {
                    checked: root.model.activateOnSingleClick
                    onToggled: root.model.activateOnSingleClick = checked
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

        SettingsSectionLabel {
            text: "Search"
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            SettingsRow {
                label: "Root file search"
                description: "Files are searched asynchronously, so if enabled you should expect a slight delay for file search results to show up."
                SettingsToggle {
                    checked: root.model.searchFilesInRoot
                    onToggled: root.model.searchFilesInRoot = checked
                }
            }

            SettingsRow {
                label: "Favicon Fetching"
                description: "The favicon provider used to load favicons where needed. Select 'None' to turn off favicon loading."
                showSeparator: false
                SearchableDropdown {
                    width: parent.width
                    items: root.model.faviconServiceItems
                    currentItem: root.model.currentFaviconService
                    onActivated: item => root.model.selectFaviconService(item.id)
                }
            }
        }

        SettingsSectionLabel {
            text: "System"
            visible: Platform.supports("inputServer")
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            visible: Platform.supports("inputServer")
            SettingsRow {
                label: "Input server"
                description: "Whether to spawn the input server at startup. This needs to be enabled in order to support snippets, paste to active window, and other features that require input monitoring or injection."
                showSeparator: false
                SettingsToggle {
                    checked: root.model.inputServerEnabled
                    onToggled: root.model.inputServerEnabled = checked
                }
            }
        }

        Item {
            Layout.preferredHeight: 24
        }
    }
}
