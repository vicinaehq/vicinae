import QtQuick
import QtQuick.Effects
import QtQuick.Layouts

Window {
    id: root
    readonly property var extModel: settings.extensionModel
    // Single source of truth for the built-in (non-extension) pages.
    readonly property var corePages: ({
            "general": {
                "title": qsTr("General"),
                "icon": "cog",
                "page": generalPage
            },
            "appearance": {
                "title": qsTr("Appearance"),
                "icon": "swatch",
                "page": appearancePage
            },
            "keybindings": {
                "title": qsTr("Keybindings"),
                "icon": "keyboard",
                "page": shortcutsPage
            },
            "advanced": {
                "title": qsTr("Advanced"),
                "icon": "wrench-screwdriver",
                "page": advancedPage
            },
            "about": {
                "title": qsTr("About"),
                "icon": "vicinae",
                "page": aboutPage
            }
        })
    readonly property var coreMeta: root.corePages[settings.currentPage] ?? null
    readonly property bool isExtensionPage: root.coreMeta === null
    readonly property string topbarTitle: root.isExtensionPage ? root.extModel.selectedTitle : root.coreMeta.title
    readonly property var topbarIconSource: root.isExtensionPage ? root.extModel.selectedIconSource : Img.builtin(root.coreMeta.icon).withFillColor(Theme.foreground)

    property bool nativeChrome: false
    property Component headerAccessory: Component {
        SettingsNavButtons {}
    }
    readonly property int sidebarWidth: 240
    readonly property real sidebarTopPadding: Style.sidebarTopInset
    readonly property real paneInset: Style.paneInset
    readonly property real paneLeftInset: Style.paneLeftInset
    readonly property real paneRadius: Style.paneRadius

    width: 980
    height: 680
    minimumWidth: 980
    minimumHeight: 600
    maximumWidth: 980
    visible: true
    color: "transparent"
    flags: Qt.Window | Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinimizeButtonHint
    title: qsTr("Vicinae Settings")

    WindowMaterial.enabled: Config.blurEnabled && Config.windowOpacity < 1
    WindowMaterial.radius: 10

    Rectangle {
        id: background
        anchors.fill: parent
        Keys.onEscapePressed: settings.close()
        color: root.nativeChrome ? "transparent" : Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
        clip: true

        Item {
            visible: root.nativeChrome
            anchors.fill: parent

            readonly property real paneWidth: root.paneInset + root.sidebarWidth - root.paneLeftInset

            Rectangle {
                width: root.paneLeftInset
                height: parent.height
                color: Theme.background
            }

            Rectangle {
                x: root.paneLeftInset
                width: parent.paneWidth
                height: root.paneInset
                color: Theme.background
            }

            Rectangle {
                x: root.paneLeftInset
                y: parent.height - root.paneInset
                width: parent.paneWidth
                height: root.paneInset
                color: Theme.background
            }

            Rectangle {
                x: root.paneInset + root.sidebarWidth
                width: parent.width - root.paneInset - root.sidebarWidth
                height: parent.height
                color: Theme.background
            }

            SourceBlendRect {
                x: root.paneLeftInset
                y: root.paneInset
                width: parent.paneWidth
                height: parent.height - 2 * root.paneInset
                radius: root.paneRadius
                backgroundColor: Theme.background
                color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.8)
                borderWidth: 1
                borderColor: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.06)
            }
        }

        RowLayout {
            anchors.fill: parent
            spacing: 0

            Item {
                Layout.fillHeight: true
                Layout.preferredWidth: root.paneInset + root.sidebarWidth

                SettingsSidebar {
                    anchors.fill: parent
                    anchors.leftMargin: root.paneLeftInset
                    anchors.topMargin: root.paneInset
                    anchors.bottomMargin: root.paneInset
                    nativeSurface: root.nativeChrome
                    topInset: root.sidebarTopPadding
                }
            }

            ViciDivider {
                visible: !root.nativeChrome
                vertical: true
                Layout.fillHeight: true
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Item {
                    id: contentHeader
                    z: 1
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 44

                    ShaderEffectSource {
                        id: headerBackdrop
                        visible: false
                        live: true
                        width: contentHeader.width
                        height: contentHeader.height
                        sourceItem: pageLoader
                        sourceRect: Qt.rect(0, 0, contentHeader.width, contentHeader.height)
                    }

                    Rectangle {
                        anchors.fill: parent
                        color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
                    }

                    MultiEffect {
                        anchors.fill: parent
                        source: headerBackdrop
                        blurEnabled: true
                        blur: 1.0
                        blurMax: 20
                    }

                    Rectangle {
                        anchors.fill: parent
                        color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.4)
                    }

                    DragHandler {
                        enabled: root.nativeChrome
                        target: null
                        onActiveChanged: if (active)
                            root.startSystemMove()
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: Math.max(16, (contentHeader.width - 720) / 2)
                        anchors.rightMargin: Math.max(16, (contentHeader.width - 720) / 2)
                        anchors.topMargin: root.nativeChrome ? 9 : 0
                        opacity: !root.nativeChrome || root.active ? 1 : 0.55
                        Behavior on opacity {
                            NumberAnimation {
                                duration: 150
                            }
                        }
                        spacing: 12

                        Loader {
                            active: root.headerAccessory !== null
                            visible: active
                            sourceComponent: root.headerAccessory
                            Layout.alignment: Qt.AlignVCenter
                        }

                        ViciImage {
                            visible: root.topbarIconSource !== "" && root.isExtensionPage
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
                                        return qsTr("Imported from Raycast");
                                    if (p === "Vicinae")
                                        return qsTr("From the Vicinae store");
                                    if (p === "Local")
                                        return qsTr("Locally installed extension");
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

                Loader {
                    id: pageLoader
                    anchors.fill: parent

                    Component.onCompleted: _loadPage(settings.currentPage)

                    function _loadPage(page) {
                        active = false;
                        const meta = root.corePages[page] ?? null;
                        if (!meta)
                            settings.extensionModel.selectProviderById(page);
                        sourceComponent = meta ? meta.page : extensionPage;
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

        Item {
            visible: root.nativeChrome
            width: root.paneInset + root.sidebarWidth
            height: root.paneInset + root.sidebarTopPadding

            DragHandler {
                target: null
                onActiveChanged: if (active)
                    root.startSystemMove()
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
        id: aboutPage
        AboutSettingsPage {}
    }

    Component {
        id: extensionPage
        ExtensionSettingsPage {}
    }
}
