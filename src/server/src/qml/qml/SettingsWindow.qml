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

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            SettingsNavBar {
                Layout.fillWidth: true
                currentIndex: settings.currentTab
                onTabClicked: (index) => settings.currentTab = index
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: Theme.divider
            }

            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: settings.currentTab

                GeneralSettingsTab {}
                ExtensionSettingsTab {}
                KeybindSettingsTab {}
                AboutSettingsTab {}
            }
        }
    }

    Shortcut {
        sequence: "Escape"
        onActivated: settings.close()
    }
}
