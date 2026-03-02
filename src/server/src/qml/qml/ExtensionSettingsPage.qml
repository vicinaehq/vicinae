import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    readonly property var extModel: settings.extensionModel
    readonly property string providerId: settings.currentPage

    Component.onCompleted: {
        root.extModel.selectProviderById(root.providerId);
        const pending = settings.pendingCommandId;
        if (!pending) return;
        settings.pendingCommandId = "";
        const row = root.extModel.commandModel.findByEntrypointId(pending);
        console.log("[ExtensionSettingsPage] pending:", pending,
                    "count:", root.extModel.commandModel.count, "row:", row);
        if (row < 0) return;
        root.expandedCommandId = pending;
        root.extModel.loadCommandPreferences(pending);
        const lv = pageLoader.item;
        if (lv) {
            lv.forceLayout();
            lv.positionViewAtIndex(row, ListView.Center);
        }
    }

    property string expandedCommandId: ""
    property string _cmdFilter: ""

    function _matchesFilter(name) {
        if (!_cmdFilter) return true
        return name.toLowerCase().includes(_cmdFilter.toLowerCase())
    }

    Loader {
        id: pageLoader
        anchors.fill: parent
        sourceComponent: {
            switch (settings.currentPage) {
            default: return defaultExtensionPage
            }
        }
    }

    Component {
        id: defaultExtensionPage

        ListView {
            id: cmdListView
            clip: true
            currentIndex: -1
            boundsBehavior: Flickable.StopAtBounds
            model: root.extModel.commandModel

            ScrollBar.vertical: ViciScrollBar {
                policy: cmdListView.contentHeight > cmdListView.height
                        ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }

            header: ColumnLayout {
                width: cmdListView.width
                spacing: 0

                readonly property real contentWidth: Math.min(width, 680)
                readonly property real sideMargin: (width - contentWidth) / 2

                Item { implicitHeight: 24 }

                ColumnLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    Layout.leftMargin: parent.sideMargin + 20
                    Layout.rightMargin: parent.sideMargin + 20
                    spacing: 8

                    ViciImage {
                        source: root.extModel.selectedIconSource
                        Layout.preferredWidth: 48
                        Layout.preferredHeight: 48
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Text {
                        text: root.extModel.selectedTitle
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize + 2
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        Layout.fillWidth: true
                    }

                    Text {
                        visible: root.extModel.selectedDescription !== ""
                        text: root.extModel.selectedDescription
                        color: Theme.textMuted
                        font.pointSize: Theme.regularFontSize
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }
                }

                Item { implicitHeight: 16 }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.leftMargin: parent.sideMargin + 20
                    Layout.rightMargin: parent.sideMargin + 20
                    height: 1
                    color: Theme.divider
                }

                // Provider preferences
                ColumnLayout {
                    visible: root.extModel.hasPreferences
                    Layout.fillWidth: true
                    Layout.leftMargin: parent.sideMargin + 20
                    Layout.rightMargin: parent.sideMargin + 20
                    Layout.topMargin: 16
                    spacing: 0

                    Text {
                        text: "Preferences"
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize
                        font.bold: true
                        Layout.bottomMargin: 12
                    }

                    SettingsPreferenceForm {
                        Layout.fillWidth: true
                        prefModel: root.extModel.preferenceModel
                    }

                    Item { implicitHeight: 16 }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: Theme.divider
                    }
                }

                // Metadata
                ColumnLayout {
                    visible: root.extModel.selectedMetadata.length > 0
                    Layout.fillWidth: true
                    Layout.leftMargin: parent.sideMargin + 20
                    Layout.rightMargin: parent.sideMargin + 20
                    Layout.topMargin: 12
                    spacing: 0

                    Repeater {
                        model: root.extModel.selectedMetadata

                        delegate: ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 0

                            RowLayout {
                                Layout.fillWidth: true
                                Layout.topMargin: 5
                                Layout.bottomMargin: 5
                                spacing: 10

                                Text {
                                    text: modelData.key
                                    color: Theme.textMuted
                                    font.pointSize: Theme.smallerFontSize
                                }

                                Item { Layout.fillWidth: true }

                                Text {
                                    text: modelData.value
                                    color: Theme.foreground
                                    font.pointSize: Theme.smallerFontSize
                                    horizontalAlignment: Text.AlignRight
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                height: 1
                                color: Theme.divider
                            }
                        }
                    }

                    Item { implicitHeight: 8 }
                }

                // Commands section title + search
                RowLayout {
                    visible: root.extModel.commandModel.count > 0
                    Layout.fillWidth: true
                    Layout.leftMargin: parent.sideMargin + 20
                    Layout.rightMargin: parent.sideMargin + 20
                    Layout.topMargin: 16
                    Layout.bottomMargin: 8
                    spacing: 8

                    Text {
                        text: "Commands"
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize
                        font.bold: true
                        Layout.fillWidth: true
                    }

                    Rectangle {
                        Layout.preferredWidth: 160
                        height: 24
                        radius: 4
                        color: "transparent"
                        border.color: cmdSearchField.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder
                        border.width: 1

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 6
                            anchors.rightMargin: 6
                            spacing: 4

                            Image {
                                source: "image://vicinae/builtin:magnifying-glass?fg=" + Theme.textMuted
                                sourceSize.width: 10
                                sourceSize.height: 10
                                Layout.preferredWidth: 10
                                Layout.preferredHeight: 10
                            }

                            TextField {
                                id: cmdSearchField
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                verticalAlignment: TextInput.AlignVCenter
                                font.pointSize: Theme.smallerFontSize
                                color: Theme.foreground
                                placeholderText: "Filter..."
                                placeholderTextColor: Theme.textPlaceholder
                                background: null
                                padding: 0
                                activeFocusOnTab: true

                                onTextChanged: root._cmdFilter = text
                                Keys.onPressed: (event) => {
                                    if (event.key === Qt.Key_Escape && text !== "") {
                                        text = ""
                                        event.accepted = true
                                    }
                                }
                            }
                        }
                    }
                }
            }

            delegate: Column {
                id: cmdDelegate
                width: cmdListView.width

                required property int index
                required property string name
                required property string type
                required property string iconSource
                required property string description
                required property bool enabled
                required property string alias
                required property string entrypointId
                required property bool hasPreferences

                readonly property real contentWidth: Math.min(width, 680)
                readonly property real sideMargin: (width - contentWidth) / 2
                readonly property bool isExpanded: hasPreferences && root.expandedCommandId === entrypointId
                readonly property bool matches: root._matchesFilter(name)

                visible: matches
                height: matches ? implicitHeight : 0

                Rectangle {
                    width: parent.width
                    height: cmdRow.implicitHeight + 16
                    color: cmdHover.hovered ? Theme.listItemHoverBg : "transparent"

                    HoverHandler { id: cmdHover }
                    TapHandler {
                        enabled: cmdDelegate.hasPreferences
                        onTapped: {
                            if (cmdDelegate.isExpanded) {
                                root.expandedCommandId = ""
                            } else {
                                root.expandedCommandId = cmdDelegate.entrypointId
                                root.extModel.loadCommandPreferences(cmdDelegate.entrypointId)
                            }
                        }
                    }

                    RowLayout {
                        id: cmdRow
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.leftMargin: cmdDelegate.sideMargin + 20
                        anchors.rightMargin: cmdDelegate.sideMargin + 20
                        spacing: 10

                        ViciImage {
                            source: Img.builtin(cmdDelegate.isExpanded ? "chevron-down-small" : "chevron-right-small")
                                .withFillColor(Theme.textMuted)
                            opacity: cmdDelegate.hasPreferences ? 1.0 : 0.25
                            Layout.preferredWidth: 16
                            Layout.preferredHeight: 16
                        }

                        ViciImage {
                            source: cmdDelegate.iconSource
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: 20
                        }

                        Text {
                            text: cmdDelegate.name
                            color: !cmdDelegate.enabled ? Theme.textMuted : Theme.foreground
                            font.pointSize: Theme.regularFontSize
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }

                        TextField {
                            id: cmdAliasInput
                            Layout.preferredWidth: 120
                            implicitHeight: 24
                            font.pointSize: Theme.smallerFontSize
                            color: Theme.foreground
                            horizontalAlignment: TextInput.AlignHCenter
                            placeholderText: "Add Alias"
                            placeholderTextColor: Theme.textPlaceholder
                            activeFocusOnTab: true
                            padding: 0
                            leftPadding: 6
                            rightPadding: 6
                            text: cmdDelegate.alias

                            background: Rectangle {
                                radius: 4
                                color: "transparent"
                                border.color: cmdAliasInput.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder
                                border.width: cmdAliasInput.activeFocus || cmdAliasInput.hovered ? 1 : 0
                            }

                            HoverHandler { id: aliasHover }

                            onActiveFocusChanged: {
                                if (!activeFocus)
                                    root.extModel.setAliasByEntrypointId(
                                        cmdDelegate.entrypointId, text)
                            }
                            onAccepted: root.extModel.setAliasByEntrypointId(
                                cmdDelegate.entrypointId, text)
                        }

                        Rectangle {
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: 20
                            radius: 4
                            color: cmdDelegate.enabled ? Theme.accent : "transparent"
                            border.color: cmdDelegate.enabled ? Theme.accent : Theme.inputBorder
                            border.width: 1

                            Text {
                                anchors.centerIn: parent
                                text: "\u2713"
                                color: "#ffffff"
                                font.pixelSize: 13
                                font.bold: true
                                visible: cmdDelegate.enabled
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: root.extModel.setEnabledByEntrypointId(
                                    cmdDelegate.entrypointId, !cmdDelegate.enabled)
                            }
                        }
                    }
                }

                // Expanded: per-command preferences only
                Rectangle {
                    visible: cmdDelegate.isExpanded
                    width: parent.width
                    implicitHeight: expandedContent.implicitHeight + 24
                    color: "transparent"

                    Rectangle {
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: cmdDelegate.sideMargin + 20
                        anchors.rightMargin: cmdDelegate.sideMargin + 20
                        height: 1
                        color: Theme.divider
                    }

                    ColumnLayout {
                        id: expandedContent
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.leftMargin: cmdDelegate.sideMargin + 52
                        anchors.rightMargin: cmdDelegate.sideMargin + 20
                        anchors.topMargin: 12
                        spacing: 8

                        SettingsPreferenceForm {
                            Layout.fillWidth: true
                            prefModel: root.extModel.commandPreferenceModel
                        }
                    }
                }

                // Row separator
                Rectangle {
                    visible: cmdDelegate.matches && index < cmdListView.count - 1
                    width: parent.width
                    height: 1
                    color: Theme.divider
                }
            }

            footer: Item { width: 1; height: 24 }
        }
    }
}
