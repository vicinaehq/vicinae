pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

Item {
    id: root
    readonly property var extModel: Settings.extensionModel
    readonly property string providerId: Settings.currentPage
    readonly property real contentWidth: Math.min(width - 32, 720)
    readonly property real sideMargin: (width - contentWidth) / 2
    property string expandedCommandId: ""
    property string _focusedCommandId: ""
    property string _focusedToolKey: ""
    readonly property string subpageRoot: {
        const subpage = Settings.currentSubpage;
        const slash = subpage.indexOf("/");
        return slash < 0 ? subpage : subpage.slice(0, slash);
    }
    readonly property url subpageUrl: {
        if (root.subpageRoot === "" || root.providerId === "")
            return "";
        const page = root.extModel.settingsPages.find(page => page.id === root.subpageRoot);
        return page ? page.component : root.extModel.preferenceModel.componentFor(root.subpageRoot);
    }
    readonly property bool showingSubpage: root.subpageUrl.toString() !== ""

    Timer {
        id: focusFlash
        interval: 2000
        onTriggered: {
            root._focusedCommandId = "";
            root._focusedToolKey = "";
        }
    }

    // Content above the list lays out async after the initial scroll; re-anchor until settled.
    Timer {
        id: scrollSettle
        interval: 800
        onTriggered: cmdFlickable.pendingScrollTarget = null
    }

    Component.onCompleted: {
        root.extModel.commandModel.setFilter("");
        root.extModel.selectProviderById(root.providerId);
        _handlePendingCommand();
        _handlePendingTool();
    }

    HoverResetOnModelChange {
        target: root.extModel ? root.extModel.commandModel : null
    }

    function _handlePendingCommand() {
        const pending = Settings.pendingCommandId;
        if (!pending)
            return;
        const row = root.extModel.commandModel.findByEntrypointId(pending);
        if (row < 0)
            return;
        Settings.pendingCommandId = "";
        root.expandedCommandId = pending;
        root._focusedCommandId = pending;
        focusFlash.restart();
        root.extModel.loadCommandPreferences(pending);
        _scrollTo(() => cmdRepeater.itemAt(row));
    }

    function _handlePendingTool() {
        const pending = Settings.pendingToolKey;
        if (!pending)
            return;
        const row = root.extModel.tools.findIndex(tool => tool.key === pending);
        if (row < 0)
            return;
        Settings.pendingToolKey = "";
        root._focusedToolKey = pending;
        focusFlash.restart();
        _scrollTo(() => toolRepeater.itemAt(row));
    }

    function _scrollTo(target) {
        cmdFlickable.pendingScrollTarget = target;
        scrollSettle.restart();
        Qt.callLater(() => {
            if (cmdFlickable)
                cmdFlickable.scrollToItem(target());
        });
    }

    Connections {
        target: Settings
        function onPendingCommandIdChanged() {
            root._handlePendingCommand();
        }
        function onPendingToolKeyChanged() {
            root._handlePendingTool();
        }
    }

    Loader {
        anchors.fill: parent
        active: root.showingSubpage
        visible: active
        source: root.subpageUrl
    }

    Flickable {
        id: cmdFlickable
        visible: !root.showingSubpage
        anchors.fill: parent
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        topMargin: Style.contentTopInset
        Component.onCompleted: contentY = -topMargin
        contentHeight: contentColumn.implicitHeight
        contentWidth: width

        // Returns the item to keep in view while the content above it lays out.
        property var pendingScrollTarget: null
        onContentHeightChanged: {
            if (pendingScrollTarget)
                Qt.callLater(() => scrollToItem(pendingScrollTarget?.()));
        }

        ViciWheelHandler {
            target: cmdFlickable
        }

        function scrollToItem(item) {
            if (!item)
                return;
            // Map into the Flickable content so the header above the list counts.
            const y = item.mapToItem(cmdFlickable.contentItem, 0, 0).y;
            contentY = Math.max(-topMargin, Math.min(y - 24 - topMargin, contentHeight - height));
        }

        ScrollBar.vertical: ViciScrollBar {
            topPadding: Style.contentTopInset
            bottomPadding: 16
            policy: cmdFlickable.contentHeight > cmdFlickable.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
        }

        ColumnLayout {
            id: contentColumn
            width: cmdFlickable.width
            spacing: 0

            ColumnLayout {
                visible: root.extModel.selectedDescription !== ""
                Layout.fillWidth: true
                Layout.leftMargin: root.sideMargin
                Layout.rightMargin: root.sideMargin
                Layout.topMargin: 24
                spacing: 0

                SettingsSectionLabel {
                    text: qsTr("Description")
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
                Layout.leftMargin: root.sideMargin
                Layout.rightMargin: root.sideMargin
                Layout.topMargin: 24
                spacing: 0

                SettingsSectionLabel {
                    text: qsTr("Preferences")
                    Layout.bottomMargin: 10
                }

                SettingsGroup {
                    SettingsPreferenceForm {
                        Layout.fillWidth: true
                        prefModel: root.extModel.preferenceModel
                    }
                }
            }

            SettingsGroup {
                visible: root.extModel.settingsPages.length > 0
                Layout.leftMargin: root.sideMargin
                Layout.rightMargin: root.sideMargin
                Layout.topMargin: 24

                Repeater {
                    model: root.extModel.settingsPages

                    delegate: SettingsSubpageRow {
                        required property var modelData
                        required property int index
                        label: modelData.title
                        description: modelData.description
                        subpage: modelData.id
                        showSeparator: index < root.extModel.settingsPages.length - 1
                    }
                }
            }

            SettingsSectionLabel {
                visible: root.extModel.commandModel.totalCount > 0
                text: qsTr("Commands")
                Layout.fillWidth: true
                Layout.leftMargin: root.sideMargin
                Layout.rightMargin: root.sideMargin
                Layout.topMargin: 24
                Layout.bottomMargin: 10
            }

            // Command list
            SettingsGroup {
                visible: root.extModel.commandModel.totalCount > 0
                Layout.leftMargin: root.sideMargin
                Layout.rightMargin: root.sideMargin

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
                        required property bool isEnabled
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

                                Item {
                                    id: titleWrap
                                    Layout.fillWidth: true
                                    Layout.minimumWidth: 0
                                    implicitHeight: titleText.implicitHeight

                                    readonly property real chevronSpace: cmdChevron.visible ? cmdChevron.width + cmdRow.spacing : 0

                                    Text {
                                        id: titleText
                                        anchors.left: parent.left
                                        anchors.verticalCenter: parent.verticalCenter
                                        width: Math.min(implicitWidth, titleWrap.width - titleWrap.chevronSpace)
                                        text: cmdDelegate.name
                                        color: !cmdDelegate.isEnabled ? Theme.textMuted : Theme.foreground
                                        font.pointSize: Theme.regularFontSize
                                        elide: Text.ElideRight
                                        maximumLineCount: 1
                                    }

                                    ViciImage {
                                        id: cmdChevron
                                        visible: cmdDelegate.hasPreferences
                                        source: Img.icon(cmdDelegate.isExpanded ? BuiltinIcon.ChevronDownSmall : BuiltinIcon.ChevronRightSmall).withFillColor(Theme.textMuted)
                                        anchors.left: titleText.right
                                        anchors.leftMargin: visible ? cmdRow.spacing : 0
                                        anchors.verticalCenter: parent.verticalCenter
                                        width: 16
                                        height: 16
                                    }
                                }

                                ShortcutField {
                                    visible: Platform.supports("globalShortcuts")
                                    bordered: false
                                    placeholder: qsTr("Shortcut")
                                    shortcutId: cmdDelegate.entrypointId
                                    shortcut: cmdDelegate.shortcut
                                    onAccepted: shortcut => root.extModel.setShortcutByEntrypointId(cmdDelegate.entrypointId, shortcut)
                                    onCleared: root.extModel.clearShortcutByEntrypointId(cmdDelegate.entrypointId)
                                }

                                InlineEditableText {
                                    Layout.preferredWidth: 120
                                    Layout.preferredHeight: 24
                                    text: cmdDelegate.alias
                                    placeholder: qsTr("Add Alias")
                                    onCommitted: value => root.extModel.setAliasByEntrypointId(cmdDelegate.entrypointId, value)
                                }

                                Rectangle {
                                    Layout.preferredWidth: 20
                                    Layout.preferredHeight: 20
                                    radius: 4
                                    color: cmdDelegate.isEnabled ? Theme.accent : "transparent"
                                    border.color: Config.withAlpha(cmdDelegate.isEnabled ? Theme.accent : Theme.inputBorder, Config.surfaceOpacity)
                                    border.width: 1

                                    Text {
                                        anchors.centerIn: parent
                                        text: "✓"
                                        color: "#ffffff"
                                        font.pixelSize: 13
                                        font.bold: true
                                        visible: cmdDelegate.isEnabled
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: root.extModel.setEnabledByEntrypointId(cmdDelegate.entrypointId, !cmdDelegate.isEnabled)
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

            SettingsSectionLabel {
                visible: root.extModel.tools.length > 0
                text: qsTr("Tools")
                Layout.fillWidth: true
                Layout.leftMargin: root.sideMargin
                Layout.rightMargin: root.sideMargin
                Layout.topMargin: 24
                Layout.bottomMargin: root.extModel.toolsAvailable ? 10 : 4
            }

            Text {
                visible: root.extModel.tools.length > 0 && !root.extModel.toolsAvailable
                text: qsTr("AI tools are turned off in the AI extension settings. Your choices below are kept.")
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                wrapMode: Text.Wrap
                Layout.fillWidth: true
                Layout.leftMargin: root.sideMargin
                Layout.rightMargin: root.sideMargin
                Layout.bottomMargin: 10
            }

            SettingsGroup {
                visible: root.extModel.tools.length > 0
                Layout.leftMargin: root.sideMargin
                Layout.rightMargin: root.sideMargin

                Repeater {
                    id: toolRepeater
                    model: root.extModel.tools

                    delegate: Item {
                        id: toolDelegate
                        required property var modelData
                        required property int index
                        Layout.fillWidth: true
                        implicitHeight: toolRow.implicitHeight

                        Rectangle {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            anchors.rightMargin: 8
                            anchors.topMargin: 2
                            anchors.bottomMargin: 2
                            radius: 8
                            color: Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.1)
                            opacity: toolDelegate.modelData.key === root._focusedToolKey ? 1 : 0
                            visible: opacity > 0
                            Behavior on opacity {
                                NumberAnimation {
                                    duration: 150
                                }
                            }
                        }

                        ToolSettingsRow {
                            id: toolRow
                            width: parent.width
                            tool: toolDelegate.modelData
                            policyItems: root.extModel.toolPolicyItems
                            showSeparator: toolDelegate.index < root.extModel.tools.length - 1
                            onPolicySelected: policy => root.extModel.setToolPolicy(toolDelegate.modelData.id, policy)
                        }
                    }
                }
            }

            Item {
                Layout.preferredHeight: 24
            }
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 4
        visible: root.extModel.selectedDescription === "" && !root.extModel.hasPreferences && root.extModel.commandModel.totalCount === 0 && root.extModel.settingsPages.length === 0 && root.extModel.tools.length === 0

        ViciImage {
            source: root.extModel.selectedIconSource
            opacity: 0.5
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 8
        }

        Text {
            text: qsTr("Nothing to configure")
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize + 1
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: qsTr("Commands and preferences will show up here once available.")
            color: Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
