import QtQuick
import QtQuick.Layouts

Window {
    id: root
    readonly property var extModel: settings.extensionModel
    readonly property bool isExtensionPage: settings.currentPage !== "general" && settings.currentPage !== "keybindings" && settings.currentPage !== "advanced" && settings.currentPage !== "about"
    readonly property string topbarTitle: {
        if (root.isExtensionPage)
            return root.extModel.selectedTitle;

        switch (settings.currentPage) {
        case "general":
            return "General";
        case "keybindings":
            return "Keybindings";
        case "advanced":
            return "Advanced";
        case "about":
            return "About";
        default:
            return "";
        }
    }
    readonly property var topbarIconSource: {
        if (root.isExtensionPage)
            return root.extModel.selectedIconSource;

        switch (settings.currentPage) {
        case "general":
            return Img.builtin("cog").withFillColor(Theme.foreground);
        case "keybindings":
            return Img.builtin("keyboard").withFillColor(Theme.foreground);
        case "advanced":
            return Img.builtin("wrench-screwdriver").withFillColor(Theme.foreground);
        case "about":
            return Img.builtin("vicinae").withFillColor(Theme.foreground);
        default:
            return "";
        }
    }

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

                // Header with page metadata and close button
                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 20
                        anchors.rightMargin: 12
                        spacing: 10

                        ViciImage {
                            visible: root.topbarIconSource !== ""
                            source: root.topbarIconSource
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: 20
                            Layout.alignment: Qt.AlignVCenter
                        }

                        Text {
                            visible: root.topbarTitle !== ""
                            text: root.topbarTitle
                            color: Theme.foreground
                            font.pointSize: Theme.regularFontSize
                            font.bold: true
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignVCenter
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        Item {
                            visible: root.isExtensionPage && root.extModel.selectedIsProvider
                            Layout.preferredWidth: visible ? headerToggle.implicitWidth : 0
                            Layout.preferredHeight: visible ? headerToggle.implicitHeight : 0
                            Layout.alignment: Qt.AlignVCenter

                            SettingsToggle {
                                id: headerToggle
                                visible: parent.visible
                                checked: root.extModel.selectedEnabled
                                onToggled: root.extModel.setEnabled(root.extModel.selectedRow, checked)
                            }
                        }

                        Rectangle {
                            id: closeBtn
                            Layout.alignment: Qt.AlignVCenter
                            width: 24
                            height: 24
                            radius: 12
                            color: closeHover.hovered ? Qt.rgba(Theme.listItemHoverBg.r, Theme.listItemHoverBg.g, Theme.listItemHoverBg.b, Config.windowOpacity) : "transparent"

                            Text {
                                anchors.centerIn: parent
                                text: "\u2715"
                                color: closeHover.hovered ? Theme.foreground : Theme.textMuted
                                font.pixelSize: 12
                            }

                            HoverHandler {
                                id: closeHover
                            }
                            TapHandler {
                                onTapped: settings.close()
                            }
                        }
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
                        if (page !== "general" && page !== "keybindings" && page !== "advanced" && page !== "about") {
                            settings.extensionModel.selectProviderById(page);
                        }
                        switch (page) {
                        case "general":
                            sourceComponent = generalPage;
                            break;
                        case "keybindings":
                            sourceComponent = shortcutsPage;
                            break;
                        case "advanced":
                            sourceComponent = advancedPage;
                            break;
                        case "about":
                            sourceComponent = aboutPage;
                            break;
                        default:
                            sourceComponent = extensionPage;
                            break;
                        }
                        active = true;
                    }

                    Connections {
                        target: settings
                        function onCurrentPageChanged() {
                            pageLoader._loadPage(settings.currentPage);
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
