import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    readonly property var model: settings.keybindModel

    property int _recordingRow: -1

    HoverResetOnModelChange {
        target: root.model
    }

    ShortcutRecorderField {
        id: recorder
        shortcutDisplayProvider: (key, mods) => root.model.shortcutDisplayTokens(key, mods)
        validateShortcut: (key, mods) => root.model.validateShortcut(root._recordingRow, key, mods)
        onShortcutCaptured: (key, modifiers) => root.model.setShortcut(root._recordingRow, key, modifiers)
    }

    Connections {
        target: recorder
        function onClosed() {
            root._recordingRow = -1;
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Header with title and search field
        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 20
            Layout.rightMargin: 20
            Layout.topMargin: 12
            Layout.bottomMargin: 8
            spacing: 8

            Text {
                text: "Keybindings"
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
                border.color: Config.withAlpha(searchField.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder, Config.windowOpacity)
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 6
                    anchors.rightMargin: 6
                    spacing: 4

                    ViciImage {
                        source: Img.builtin("magnifying-glass").withFillColor(Theme.textMuted)
                        sourceSize.width: 10
                        sourceSize.height: 10
                        Layout.preferredWidth: 10
                        Layout.preferredHeight: 10
                    }

                    TextField {
                        id: searchField
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

                        Timer {
                            id: filterDebounce
                            interval: 16
                            onTriggered: root.model.setFilter(searchField.text)
                        }

                        onTextChanged: filterDebounce.restart()
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

        ViciDivider {
            Layout.fillWidth: true
        }

        ListView {
            id: keybindList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.model
            boundsBehavior: Flickable.StopAtBounds

            ScrollBar.vertical: ViciScrollBar {
                policy: keybindList.contentHeight > keybindList.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }

            delegate: Item {
                id: rowItem
                width: keybindList.width
                height: 45

                required property int index
                required property string name
                required property string icon
                required property var shortcutTokens

                readonly property bool isRecording: index === root._recordingRow

                Rectangle {
                    anchors.fill: parent
                    anchors.bottomMargin: 1
                    color: rowItem.isRecording ? Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.08) : (rowHover.hovered && HoverActivation.active) ? Theme.listItemHoverBg : "transparent"
                }

                HoverHandler {
                    id: rowHover
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (recorder.show(rowItem, true))
                            root._recordingRow = rowItem.index;
                    }
                }

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 12

                    ViciImage {
                        source: rowItem.icon ? Img.builtin(rowItem.icon).withBackgroundTint("accent") : ""
                        Layout.preferredWidth: 20
                        Layout.preferredHeight: 20
                        visible: rowItem.icon !== ""
                    }

                    Text {
                        text: rowItem.name
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }

                    ShortcutBadge {
                        visible: rowItem.shortcutTokens.length > 0
                        tokens: rowItem.shortcutTokens
                    }

                    Text {
                        visible: !rowItem.isRecording && rowItem.shortcutTokens.length === 0 && rowHover.hovered && HoverActivation.active
                        text: "Record Shortcut"
                        color: Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                    }
                }

                ViciDivider {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                }
            }
        }
    }
}
