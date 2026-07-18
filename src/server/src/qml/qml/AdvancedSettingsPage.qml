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
                label: "Wrap navigation"
                description: "Wrap around to the opposite end when moving past the first or last item."
                SettingsToggle {
                    checked: root.model.wrapNavigation
                    onToggled: root.model.wrapNavigation = checked
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
                description: Qt.platform.os === "osx" ? "Default uses the standard macOS keys (arrows, Ctrl+N/P); Vim uses Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Opt+B/F for navigation, plus Emacs editing in the search bar." : "Default and Vim use Ctrl+J/K and Ctrl+H/L; Emacs uses Ctrl+N/P and Ctrl+Alt+B/F for navigation, plus Emacs editing in the search bar."
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

        SettingsSectionLabel {
            text: "Security"
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            SettingsRow {
                label: "Encrypt sensitive data"
                description: "Encrypt sensitive data at rest, such as clipboard history and internal databases (OAuth tokens, extension local storage, API keys). Note that some components, such as on-disk clipboard history, may not be retroactively affected when toggling this option. Turning on this option may ask you to unlock your keychain. Requires a restart in order to apply."
                showSeparator: false
                SettingsToggle {
                    checked: root.model.encryptSensitiveData
                    onToggled: checked => root.model.encryptSensitiveData = checked
                }
            }
        }

        Item {
            Layout.preferredHeight: 24
        }
    }
}
