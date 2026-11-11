pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

// Web search settings, opened from the AI extension's "Web search" row.
Flickable {
    id: root

    readonly property AISettingsModel model: Settings.aiModel
    readonly property var currentProvider: root.model.webSearchProviders.find(p => p.id === root.model.webSearchProvider) ?? null
    readonly property bool hasTestResult: root.model.webSearchTestStatus !== ""
    readonly property bool keyGood: root.hasTestResult ? root.model.webSearchTestSucceeded : root.model.webSearchConfigured

    contentWidth: width
    contentHeight: column.implicitHeight
    clip: true
    boundsBehavior: Flickable.StopAtBounds
    topMargin: Style.contentTopInset
    Component.onCompleted: contentY = -topMargin

    ViciWheelHandler {
        target: root
    }

    ScrollBar.vertical: ViciScrollBar {
        topPadding: Style.contentTopInset
        bottomPadding: 16
        policy: root.contentHeight > root.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
    }

    ColumnLayout {
        id: column
        width: root.width
        spacing: 0

        readonly property real contentWidth: Math.min(width - 32, 760)
        readonly property real sideMargin: (width - contentWidth) / 2

        Item {
            implicitHeight: 16
        }

        Rectangle {
            Layout.leftMargin: column.sideMargin
            implicitWidth: backRow.implicitWidth + 12
            implicitHeight: 28
            radius: 6
            color: backHover.hovered ? Theme.listItemHoverBg : "transparent"

            RowLayout {
                id: backRow
                anchors.centerIn: parent
                spacing: 4

                ViciImage {
                    source: Img.icon(BuiltinIcon.ChevronRightSmall).withFillColor(Theme.textMuted)
                    Layout.preferredWidth: 14
                    Layout.preferredHeight: 14
                    rotation: 180
                }

                Text {
                    text: Settings.extensionModel.selectedTitle
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize
                }
            }

            HoverHandler {
                id: backHover
                cursorShape: Qt.PointingHandCursor
            }

            TapHandler {
                onTapped: Settings.currentSubpage = ""
            }
        }

        SettingsSectionLabel {
            text: qsTr("Web search")
            Layout.leftMargin: column.sideMargin
            Layout.topMargin: 16
            Layout.bottomMargin: 4
        }

        Text {
            text: qsTr("Let AI Chat look up current information. Results come from the search service you choose below.")
            color: Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            wrapMode: Text.Wrap
            Layout.fillWidth: true
            Layout.leftMargin: column.sideMargin
            Layout.rightMargin: column.sideMargin
            Layout.bottomMargin: 14
        }

        SettingsGroup {
            Layout.leftMargin: column.sideMargin
            Layout.rightMargin: column.sideMargin

            SettingsRow {
                label: qsTr("Enable web search")
                description: qsTr("Search the web when a question needs current information.")
                showSeparator: false
                controlWidth: enableToggle.implicitWidth

                SettingsToggle {
                    id: enableToggle
                    checked: root.model.webSearchEnabled
                    onToggled: checked => root.model.setWebSearchEnabled(checked)
                }
            }
        }

        SettingsSectionLabel {
            visible: root.model.webSearchEnabled
            text: qsTr("Search service")
            Layout.leftMargin: column.sideMargin
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            visible: root.model.webSearchEnabled
            Layout.leftMargin: column.sideMargin
            Layout.rightMargin: column.sideMargin

            SettingsRow {
                label: qsTr("Service")
                description: root.currentProvider?.description ?? ""

                SearchableDropdown {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width
                    compact: true
                    placeholder: qsTr("Choose a service")
                    items: [
                        {
                            items: root.model.webSearchProviders.map(p => ({
                                        id: p.id,
                                        displayName: p.displayName,
                                        iconSource: ""
                                    }))
                        }
                    ]
                    currentItem: root.currentProvider ? {
                        id: root.currentProvider.id,
                        displayName: root.currentProvider.displayName
                    } : null
                    onActivated: item => root.model.setWebSearchProvider(item.id)
                }
            }

            // API key: the input needs more room than a row's control slot gives it.
            ColumnLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                Layout.topMargin: 13
                Layout.bottomMargin: 13
                spacing: 10

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 20

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 3

                        Text {
                            text: qsTr("API key")
                            color: Theme.foreground
                            font.pointSize: Theme.regularFontSize
                        }

                        Text {
                            text: qsTr("Saved on this device. Each service keeps its own key.")
                            color: Theme.textMuted
                            font.pointSize: Theme.smallerFontSize
                            wrapMode: Text.Wrap
                            Layout.fillWidth: true
                        }
                    }

                    Text {
                        visible: root.currentProvider !== null
                        text: qsTr("Get an API key")
                        color: keyLinkMouse.containsMouse ? Theme.foreground : Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                        font.underline: keyLinkMouse.containsMouse

                        MouseArea {
                            id: keyLinkMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: Qt.openUrlExternally(root.currentProvider.keyUrl)
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    FormPasswordInput {
                        id: apiKeyInput
                        Layout.fillWidth: true
                        accessibleLabel: qsTr("API key")
                        placeholder: root.model.webSearchConfigured ? qsTr("Replace saved API key") : qsTr("Paste your API key")

                        function save() {
                            if (text.trim() === "")
                                return;
                            root.model.setWebSearchApiKey(text);
                            text = "";
                        }

                        onAccepted: save()
                        onEditingChanged: {
                            if (!editing)
                                save();
                        }
                    }

                    ViciButton {
                        text: qsTr("Test search")
                        variant: "ghost"
                        bordered: true
                        busy: root.model.webSearchTesting
                        enabled: !root.model.webSearchTesting
                        onClicked: {
                            apiKeyInput.save();
                            root.model.testWebSearch();
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 6

                    ViciImage {
                        visible: root.hasTestResult || root.model.webSearchConfigured
                        source: Img.icon(root.keyGood ? BuiltinIcon.Check : BuiltinIcon.Warning).withFillColor(root.keyGood ? Theme.toastSuccess : Theme.danger)
                        Layout.preferredWidth: 14
                        Layout.preferredHeight: 14
                    }

                    Text {
                        text: {
                            if (root.hasTestResult)
                                return root.model.webSearchTestStatus;
                            return root.model.webSearchConfigured ? qsTr("Key saved") : qsTr("No API key saved");
                        }
                        color: root.hasTestResult || root.model.webSearchConfigured ? (root.keyGood ? Theme.toastSuccess : Theme.danger) : Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }
                }
            }
        }

        SettingsSectionLabel {
            visible: root.model.webSearchEnabled
            text: qsTr("Results")
            Layout.leftMargin: column.sideMargin
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            visible: root.model.webSearchEnabled
            Layout.leftMargin: column.sideMargin
            Layout.rightMargin: column.sideMargin

            SettingsRow {
                label: qsTr("Search depth")
                description: qsTr("Deeper searches return more and better results, but take longer.")
                showSeparator: false
                controlWidth: depthControl.implicitWidth

                SettingsSegmentedControl {
                    id: depthControl
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    accessibleLabel: qsTr("Search depth")
                    currentId: root.model.webSearchDepth
                    options: root.model.webSearchDepths
                    onActivated: id => root.model.setWebSearchDepth(id)
                }
            }
        }

        Item {
            Layout.preferredHeight: 24
        }
    }
}
