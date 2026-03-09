import QtQuick
import QtQuick.Layouts

Window {
    id: root
    readonly property var extModel: settings.extensionModel
    // Single source of truth for the built-in (non-extension) pages.
    readonly property var corePages: ({
            "general": {
                "title": "General",
                "icon": "cog",
                "page": generalPage
            },
            "appearance": {
                "title": "Appearance",
                "icon": "swatch",
                "page": appearancePage
            },
            "keybindings": {
                "title": "Keybindings",
                "icon": "keyboard",
                "page": shortcutsPage
            },
            "advanced": {
                "title": "Advanced",
                "icon": "wrench-screwdriver",
                "page": advancedPage
            },
            "about": {
                "title": "About",
                "icon": "vicinae",
                "page": aboutPage
            }
        })
    readonly property var coreMeta: root.corePages[settings.currentPage] ?? null
    readonly property bool isExtensionPage: root.coreMeta === null
    readonly property string topbarTitle: root.isExtensionPage ? root.extModel.selectedTitle : root.coreMeta.title
    readonly property var topbarIconSource: root.isExtensionPage ? root.extModel.selectedIconSource : Img.builtin(root.coreMeta.icon).withFillColor(Theme.foreground)

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

    WindowMaterial.enabled: Config.blurEnabled
    WindowMaterial.radius: 10

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
                        if (page !== "general" && page !== "keybindings" && page !== "advanced" && page !== "ai" && page !== "about") {
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
                        case "ai":
                            sourceComponent = aiPage;
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
        id: appearancePage
        AppearanceSettingsPage {}
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
        id: aiPage
        AISettingsPage {}
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
