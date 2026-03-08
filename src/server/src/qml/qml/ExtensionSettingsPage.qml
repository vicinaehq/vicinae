import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    readonly property var extModel: settings.extensionModel
    readonly property string providerId: settings.currentPage

    Component.onCompleted: {
        root.extModel.commandModel.setFilter("");
        root.extModel.selectProviderById(root.providerId);
        _handlePendingCommand();
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

    property string expandedCommandId: ""

    Flickable {
        id: cmdFlickable
        anchors.fill: parent
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        property real _minContentHeight: 0
        contentHeight: Math.max(contentColumn.implicitHeight, _minContentHeight)
        contentWidth: width

        function scrollToIndex(row) {
            const item = cmdRepeater.itemAt(row);
            if (!item)
                return;
            contentY = Math.max(0, Math.min(item.y - (height - item.height) / 2, contentHeight - height));
        }

        ScrollBar.vertical: ViciScrollBar {
            policy: cmdFlickable.contentHeight > cmdFlickable.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
        }

        ColumnLayout {
            id: contentColumn
            width: cmdFlickable.width
            spacing: 0

            readonly property real contentWidth: Math.min(width, 680)
            readonly property real sideMargin: (width - contentWidth) / 2

            Item {
                implicitHeight: 24
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.leftMargin: contentColumn.sideMargin + 20
                Layout.rightMargin: contentColumn.sideMargin + 20
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

                Item {
                    visible: root.extModel.selectedIsProvider
                    Layout.fillWidth: true
                    Layout.topMargin: 4
                    implicitHeight: enableToggle.height

                    Item {
                        id: enableToggle
                        property bool checked: root.extModel.selectedEnabled
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 36
                        height: 20

                        Rectangle {
                            anchors.fill: parent
                            radius: 10
                            color: enableToggle.checked ? Theme.accent : Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.2)
                            Behavior on color {
                                ColorAnimation {
                                    duration: 120
                                }
                            }

                            Rectangle {
                                width: 16
                                height: 16
                                radius: 8
                                x: enableToggle.checked ? parent.width - width - 2 : 2
                                anchors.verticalCenter: parent.verticalCenter
                                color: "#ffffff"
                                Behavior on x {
                                    NumberAnimation {
                                        duration: 120
                                    }
                                }
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.extModel.setEnabled(root.extModel.selectedRow, !enableToggle.checked)
                        }
                    }
                }
            }

            Item {
                visible: root.extModel.hasPreferences || root.extModel.commandModel.totalCount > 0
                implicitHeight: 16
            }

            Rectangle {
                visible: root.extModel.hasPreferences || root.extModel.commandModel.totalCount > 0
                Layout.fillWidth: true
                Layout.leftMargin: contentColumn.sideMargin + 20
                Layout.rightMargin: contentColumn.sideMargin + 20
                height: 1
                color: Theme.divider
            }

            // Provider preferences
            ColumnLayout {
                visible: root.extModel.hasPreferences
                Layout.fillWidth: true
                Layout.leftMargin: contentColumn.sideMargin + 20
                Layout.rightMargin: contentColumn.sideMargin + 20
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

                Item {
                    implicitHeight: 16
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: Theme.divider
                }
            }

            // Commands section title + search
            RowLayout {
                visible: root.extModel.commandModel.totalCount > 0
                Layout.fillWidth: true
                Layout.leftMargin: contentColumn.sideMargin + 20
                Layout.rightMargin: contentColumn.sideMargin + 20
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

                            onTextChanged: {
                                cmdFlickable._minContentHeight = cmdFlickable.contentHeight;
                                root.extModel.commandModel.setFilter(text);
                            }
                            onActiveFocusChanged: {
                                if (!activeFocus)
                                    cmdFlickable._minContentHeight = 0;
                            }
                            Keys.onPressed: event => {
                                if (event.key === Qt.Key_Escape && text !== "") {
                                    text = "";
                                    event.accepted = true;
                                }
                            }
                        }
                    }
                }
            }

            // Command list
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

                    readonly property bool isExpanded: root.expandedCommandId === entrypointId

                    Rectangle {
                        width: parent.width
                        height: cmdRow.implicitHeight + 16
                        color: cmdHover.hovered ? Theme.listItemHoverBg : "transparent"

                        HoverHandler {
                            id: cmdHover
                        }
                        TapHandler {
                            onTapped: {
                                if (cmdDelegate.isExpanded) {
                                    root.expandedCommandId = "";
                                } else {
                                    root.expandedCommandId = cmdDelegate.entrypointId;
                                    if (cmdDelegate.hasPreferences)
                                        root.extModel.loadCommandPreferences(cmdDelegate.entrypointId);
                                }
                            }
                        }

                        RowLayout {
                            id: cmdRow
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.leftMargin: contentColumn.sideMargin + 20
                            anchors.rightMargin: contentColumn.sideMargin + 20
                            spacing: 10

                            ViciImage {
                                source: Img.builtin(cmdDelegate.isExpanded ? "chevron-down-small" : "chevron-right-small").withFillColor(Theme.textMuted)
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
                                activeFocusOnTab: false
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

                                HoverHandler {
                                    id: aliasHover
                                }

                                onActiveFocusChanged: {
                                    if (!activeFocus)
                                        root.extModel.setAliasByEntrypointId(cmdDelegate.entrypointId, text);
                                }
                                onAccepted: root.extModel.setAliasByEntrypointId(cmdDelegate.entrypointId, text)
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
                                    onClicked: root.extModel.setEnabledByEntrypointId(cmdDelegate.entrypointId, !cmdDelegate.enabled)
                                }
                            }
                        }
                    }

                    // Expanded section (lazy)
                    Loader {
                        active: cmdDelegate.isExpanded
                        visible: active
                        width: parent.width

                        sourceComponent: Rectangle {
                            implicitHeight: expandedContent.implicitHeight + 24
                            color: "transparent"

                            Rectangle {
                                anchors.top: parent.top
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.leftMargin: contentColumn.sideMargin + 20
                                anchors.rightMargin: contentColumn.sideMargin + 20
                                height: 1
                                color: Theme.divider
                            }

                            ColumnLayout {
                                id: expandedContent
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.top: parent.top
                                anchors.leftMargin: contentColumn.sideMargin + 52
                                anchors.rightMargin: contentColumn.sideMargin + 20
                                anchors.topMargin: 12
                                spacing: 8

                                SettingsPreferenceForm {
                                    visible: cmdDelegate.hasPreferences
                                    Layout.fillWidth: true
                                    prefModel: root.extModel.commandPreferenceModel
                                }

                                Text {
                                    visible: !cmdDelegate.hasPreferences
                                    text: "No configurable preferences for this command."
                                    color: Theme.textMuted
                                    font.pointSize: Theme.smallerFontSize
                                }
                            }
                        }
                    }

                    // Row separator
                    Rectangle {
                        visible: cmdDelegate.index < root.extModel.commandModel.count - 1
                        width: parent.width
                        height: 1
                        color: Theme.divider
                    }
                }
            }

            Item {
                implicitHeight: 24
            }
        }
    }
}
