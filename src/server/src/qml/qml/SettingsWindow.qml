import QtQuick
import QtQuick.Layouts

Window {
    id: root
    width: 860
    height: 540
    minimumWidth: 860
    minimumHeight: 540
    maximumWidth: 860
    maximumHeight: 540
    visible: true
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.Window
    title: "Vicinae Settings"

    Rectangle {
        id: background
        anchors.fill: parent
        radius: 10
        Keys.onEscapePressed: settings.close()
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

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 0

                // Header with close button
                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40

                    Rectangle {
                        id: closeBtn
                        anchors.right: parent.right
                        anchors.rightMargin: 12
                        anchors.verticalCenter: parent.verticalCenter
                        width: 24
                        height: 24
                        radius: 12
                        color: closeHover.hovered ? Theme.listItemHoverBg : "transparent"

                        Text {
                            anchors.centerIn: parent
                            text: "\u2715"
                            color: closeHover.hovered ? Theme.foreground : Theme.textMuted
                            font.pixelSize: 12
                        }

                        HoverHandler { id: closeHover }
                        TapHandler { onTapped: settings.close() }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: Theme.divider
                }

                Loader {
                    id: pageLoader
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Component.onCompleted: _loadPage(settings.currentPage)

                    function _loadPage(page) {
                        active = false;
                        if (page !== "general" && page !== "keybindings"
                            && page !== "advanced" && page !== "about") {
                            settings.extensionModel.selectProviderById(page)
                        }
                        switch (page) {
                        case "general": sourceComponent = generalPage; break;
                        case "keybindings": sourceComponent = shortcutsPage; break;
                        case "advanced": sourceComponent = advancedPage; break;
                        case "about": sourceComponent = aboutPage; break;
                        default: sourceComponent = extensionPage; break;
                        }
                        active = true;
                    }

                    Connections {
                        target: settings
                        function onCurrentPageChanged() {
                            pageLoader._loadPage(settings.currentPage)
                        }
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

}
