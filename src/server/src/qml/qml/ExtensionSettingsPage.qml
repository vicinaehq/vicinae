import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    readonly property var extModel: settings.extensionModel
    readonly property string providerId: settings.currentPage
    readonly property real contentWidth: Math.min(width, 720)
    readonly property real sideMargin: (width - contentWidth) / 2
    property string expandedCommandId: ""
    property string _focusedCommandId: ""

    Timer {
        id: focusFlash
        interval: 2000
        onTriggered: root._focusedCommandId = ""
    }

    Component.onCompleted: {
        root.extModel.commandModel.setFilter("");
        root.extModel.selectProviderById(root.providerId);
        _handlePendingCommand();
    }

    HoverResetOnModelChange {
        target: root.extModel ? root.extModel.commandModel : null
    }

    function _handlePendingCommand() {
        const pending = settings.pendingCommandId;
        if (!pending)
            return;
        const row = root.extModel.commandModel.findByEntrypointId(pending);
        if (row < 0)
            return;
        settings.pendingCommandId = "";
        root.expandedCommandId = pending;
        root._focusedCommandId = pending;
        focusFlash.restart();
        root.extModel.loadCommandPreferences(pending);
        Qt.callLater(() => {
            if (cmdFlickable)
                cmdFlickable.scrollToIndex(row);
        });
    }

    Connections {
        target: settings
        function onPendingCommandIdChanged() {
            root._handlePendingCommand();
        }
    }

    Flickable {
        id: cmdFlickable
        anchors.fill: parent
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        contentHeight: contentColumn.implicitHeight
        contentWidth: width

        function scrollToIndex(row) {
            const item = cmdRepeater.itemAt(row);
            if (!item)
                return;
            // Map into the Flickable content so the header above the list counts.
            const y = item.mapToItem(cmdFlickable.contentItem, 0, 0).y;
            contentY = Math.max(0, Math.min(y - 24, contentHeight - height));
        }

        ScrollBar.vertical: ViciScrollBar {
            policy: cmdFlickable.contentHeight > cmdFlickable.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
        }

        ColumnLayout {
            id: contentColumn
            width: cmdFlickable.width
            spacing: 0

            ColumnLayout {
                visible: root.extModel.selectedDescription !== ""
                Layout.fillWidth: true
                Layout.leftMargin: root.sideMargin + 20
                Layout.rightMargin: root.sideMargin + 20
                Layout.topMargin: 24
                spacing: 0

                SettingsSectionLabel {
                    text: "Description"
                    Layout.bottomMargin: 10
                }

                Text {
                    text: root.extModel.selectedDescription
                    color: Theme.textMuted
                    font.pointSize: Theme.regularFontSize
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }
            }

            // Provider preferences
            ColumnLayout {
                visible: root.extModel.hasPreferences
                Layout.fillWidth: true
                Layout.leftMargin: root.sideMargin + 20
                Layout.rightMargin: root.sideMargin + 20
                Layout.topMargin: 24
                spacing: 0

                SettingsSectionLabel {
                    text: "Preferences"
                    Layout.bottomMargin: 10
                }

                SettingsGroup {
                    SettingsPreferenceForm {
                        Layout.fillWidth: true
                        prefModel: root.extModel.preferenceModel
                    }
                }
            }

            SettingsSectionLabel {
                visible: root.extModel.commandModel.totalCount > 0
                text: "Commands"
                Layout.fillWidth: true
                Layout.leftMargin: root.sideMargin + 20
                Layout.rightMargin: root.sideMargin + 20
                Layout.topMargin: 24
                Layout.bottomMargin: 10
            }

            // Command list
            SettingsGroup {
                visible: root.extModel.commandModel.totalCount > 0
                Layout.leftMargin: root.sideMargin + 20
                Layout.rightMargin: root.sideMargin + 20

                Repeater {
                    id: cmdRepeater
                    model: root.extModel.commandModel

                    delegate: Column {
                        id: cmdDelegate
                        Layout.fillWidth: true

                        required property int index
                        required property string name
                        required property string type
                        required property string iconSource
                        required property string description
                        required property bool enabled
                        required property string alias
                        required property string entrypointId
                        required property bool hasPreferences
                        required property string shortcut

                        readonly property bool isExpanded: root.expandedCommandId === entrypointId

                        Item {
                            width: parent.width
                            height: cmdRow.implicitHeight + 16

                            Rectangle {
                                anchors.fill: parent
                                anchors.leftMargin: 8
                                anchors.rightMargin: 8
                                anchors.topMargin: 2
                                anchors.bottomMargin: 2
                                radius: 8
                                color: Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.1)
                                opacity: cmdDelegate.entrypointId === root._focusedCommandId ? 1 : 0
                                visible: opacity > 0
                                Behavior on opacity {
                                    NumberAnimation {
                                        duration: 150
                                    }
                                }
                            }

                            TapHandler {
                                enabled: cmdDelegate.hasPreferences
                                onTapped: {
                                    if (cmdDelegate.isExpanded) {
                                        root.expandedCommandId = "";
                                    } else {
                                        root.expandedCommandId = cmdDelegate.entrypointId;
                                        root.extModel.loadCommandPreferences(cmdDelegate.entrypointId);
                                    }
                                }
                            }

                            RowLayout {
                                id: cmdRow
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 16
                                anchors.rightMargin: 16
                                spacing: 10

                                ViciImage {
                                    source: cmdDelegate.iconSource
                                    Layout.preferredWidth: 22
                                    Layout.preferredHeight: 22
                                }

                                Text {
                                    text: cmdDelegate.name
                                    color: !cmdDelegate.enabled ? Theme.textMuted : Theme.foreground
                                    font.pointSize: Theme.regularFontSize
                                    elide: Text.ElideRight
                                    Layout.maximumWidth: cmdRow.width * 0.55
                                }

                                ViciImage {
                                    visible: cmdDelegate.hasPreferences
                                    source: Img.builtin(cmdDelegate.isExpanded ? "chevron-down-small" : "chevron-right-small").withFillColor(Theme.textMuted)
                                    Layout.preferredWidth: 16
                                    Layout.preferredHeight: 16
                                }

                                Item {
                                    Layout.fillWidth: true
                                }

                                ShortcutField {
                                    visible: settings.globalShortcutsSupported
                                    bordered: false
                                    placeholder: "Shortcut"
                                    shortcutId: cmdDelegate.entrypointId
                                    shortcut: cmdDelegate.shortcut
                                    onAccepted: shortcut => root.extModel.setShortcutByEntrypointId(cmdDelegate.entrypointId, shortcut)
                                    onCleared: root.extModel.clearShortcutByEntrypointId(cmdDelegate.entrypointId)
                                }

                                InlineEditableText {
                                    Layout.preferredWidth: 120
                                    Layout.preferredHeight: 24
                                    text: cmdDelegate.alias
                                    placeholder: "Add Alias"
                                    onCommitted: value => root.extModel.setAliasByEntrypointId(cmdDelegate.entrypointId, value)
                                }

                                Rectangle {
                                    Layout.preferredWidth: 20
                                    Layout.preferredHeight: 20
                                    radius: 4
                                    color: cmdDelegate.enabled ? Theme.accent : "transparent"
                                    border.color: Config.withAlpha(cmdDelegate.enabled ? Theme.accent : Theme.inputBorder, Config.windowOpacity)
                                    border.width: 1

                                    Text {
                                        anchors.centerIn: parent
                                        text: "✓"
                                        color: "#ffffff"
                                        font.pixelSize: 13
                                        font.bold: true
                                        visible: cmdDelegate.enabled
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: root.extModel.setEnabledByEntrypointId(cmdDelegate.entrypointId, !cmdDelegate.enabled)
                                    }
                                }
                            }
                        }

                        // Expanded preferences (lazy)
                        Loader {
                            active: cmdDelegate.isExpanded && cmdDelegate.hasPreferences
                            visible: active
                            width: parent.width

                            sourceComponent: Rectangle {
                                implicitHeight: expandedContent.implicitHeight + 24
                                color: "transparent"

                                ViciDivider {
                                    anchors.top: parent.top
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.leftMargin: 16
                                    anchors.rightMargin: 16
                                }

                                ColumnLayout {
                                    id: expandedContent
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.top: parent.top
                                    anchors.leftMargin: 34
                                    anchors.rightMargin: 16
                                    anchors.topMargin: 12
                                    spacing: 8

                                    SettingsPreferenceForm {
                                        Layout.fillWidth: true
                                        prefModel: root.extModel.commandPreferenceModel
                                    }
                                }
                            }
                        }

                        ViciDivider {
                            visible: cmdDelegate.index < root.extModel.commandModel.count - 1
                            x: 16
                            width: parent.width - 32
                        }
                    }
                }
            }

            Item {
                Layout.preferredHeight: 24
            }
        }
    }
}
