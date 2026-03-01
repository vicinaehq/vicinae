import QtQuick
import QtQuick.Layouts

Window {
    id: root
    width: 1000
    height: 600
    minimumWidth: 1000
    minimumHeight: 600
    maximumWidth: 1000
    maximumHeight: 600
    visible: true
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.Window
    title: "Vicinae Settings"

    Rectangle {
        id: background
        anchors.fill: parent
        radius: 10
        color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
        border.color: Theme.divider
        border.width: 1
        clip: true

        RowLayout {
            anchors.fill: parent
            spacing: 0

            SettingsSidebar {
                Layout.fillHeight: true
                Layout.preferredWidth: 220
            }

            Rectangle {
                Layout.fillHeight: true
                width: 1
                color: Theme.divider
            }

            Loader {
                Layout.fillWidth: true
                Layout.fillHeight: true
                sourceComponent: {
                    switch (settings.currentPage) {
                    case "general": return generalPage
                    case "shortcuts": return shortcutsPage
                    case "advanced": return advancedPage
                    case "about": return aboutPage
                    default: return extensionPage
                    }
                }
            }
        }
    }

    Component {
        id: generalPage
        GeneralSettingsPage {}
    }

    Component {
        id: shortcutsPage
        ShortcutsSettingsPage {}
    }

    Component {
        id: advancedPage
        AdvancedSettingsPage {}
    }

    Component {
        id: aboutPage
        AboutSettingsPage {}
    }

    Component {
        id: extensionPage
        ExtensionSettingsPage {}
    }

    Shortcut {
        sequence: "Escape"
        onActivated: settings.close()
    }
}
