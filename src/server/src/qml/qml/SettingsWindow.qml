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

    width: 980
    height: 680
    minimumWidth: 980
    minimumHeight: 680
    maximumWidth: 980
    maximumHeight: 680
    visible: true
    color: "transparent"
    // Server-side decorations: the compositor / OS owns the frame.
    flags: Qt.Window
    title: "Vicinae Settings"

    BackgroundEffect.enabled: Config.blurEnabled
    BackgroundEffect.radius: 10

    Rectangle {
        id: background
        anchors.fill: parent
        Keys.onEscapePressed: settings.close()
        color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
        clip: true

        RowLayout {
            anchors.fill: parent
            spacing: 0

            SettingsSidebar {
                Layout.fillHeight: true
                Layout.preferredWidth: 220
            }

            ViciDivider {
                vertical: true
                Layout.fillHeight: true
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 0

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 24
                        anchors.rightMargin: 16
                        spacing: 12

                        ViciImage {
                            visible: root.topbarIconSource !== ""
                            source: root.topbarIconSource
                            Layout.preferredWidth: 22
                            Layout.preferredHeight: 22
                            Layout.alignment: Qt.AlignVCenter
                        }

                        Text {
                            visible: root.topbarTitle !== ""
                            text: root.topbarTitle
                            color: Theme.foreground
                            font.pointSize: Theme.regularFontSize + 1
                            font.bold: true
                            elide: Text.ElideRight
                            Layout.maximumWidth: root.width * 0.5
                            Layout.alignment: Qt.AlignVCenter
                        }

                        ViciImage {
                            id: provenanceIcon
                            visible: {
                                const p = root.extModel.selectedProvenance;
                                return root.isExtensionPage && (p === "Raycast" || p === "Vicinae" || p === "Local");
                            }
                            source: {
                                const p = root.extModel.selectedProvenance;
                                if (p === "Raycast")
                                    return Img.builtin("raycast").withFillColor(Theme.toastDanger);
                                if (p === "Vicinae")
                                    return Img.builtin("vicinae").withFillColor(Theme.toastWarning);
                                if (p === "Local")
                                    return Img.builtin("box").withFillColor(Theme.toastInfo);
                                return "";
                            }
                            Layout.preferredWidth: 16
                            Layout.preferredHeight: 16
                            Layout.alignment: Qt.AlignVCenter

                            HoverHandler {
                                id: provenanceHover
                            }

                            ViciToolTip {
                                text: {
                                    const p = root.extModel.selectedProvenance;
                                    if (p === "Raycast")
                                        return "Imported from Raycast";
                                    if (p === "Vicinae")
                                        return "From the Vicinae store";
                                    if (p === "Local")
                                        return "Locally installed extension";
                                    return "";
                                }
                                visible: provenanceHover.hovered && text !== ""
                            }
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
                    }
                }

                ViciDivider {
                    Layout.fillWidth: true
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
                            HoverActivation.reset();
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
