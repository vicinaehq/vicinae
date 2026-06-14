import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    readonly property var model: settings.keybindModel
    readonly property real contentWidth: Math.min(width, 720)
    readonly property real sideMargin: (width - contentWidth) / 2

    property int _recordingRow: -1

    Component.onCompleted: root.model.setFilter("")

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

    Flickable {
        anchors.fill: parent
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        contentHeight: contentColumn.implicitHeight
        contentWidth: width

        ScrollBar.vertical: ViciScrollBar {
            policy: contentHeight > height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
        }

        ColumnLayout {
            id: contentColumn
            width: parent.width
            spacing: 0

            SettingsSectionLabel {
                text: "Keybindings"
                Layout.fillWidth: true
                Layout.leftMargin: root.sideMargin + 20
                Layout.rightMargin: root.sideMargin + 20
                Layout.topMargin: 24
                Layout.bottomMargin: 10
            }

            SettingsGroup {
                Layout.leftMargin: root.sideMargin + 20
                Layout.rightMargin: root.sideMargin + 20

                Repeater {
                    id: keybindRepeater
                    model: root.model

                    delegate: Column {
                        id: rowItem
                        Layout.fillWidth: true

                        required property int index
                        required property string name
                        required property string icon
                        required property var shortcutTokens

                        readonly property bool isRecording: index === root._recordingRow

                        Item {
                            width: parent.width
                            height: kbRow.implicitHeight + 16

                            Rectangle {
                                anchors.fill: parent
                                anchors.topMargin: 1
                                anchors.bottomMargin: 1
                                visible: rowItem.isRecording
                                color: Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.08)
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (recorder.show(rowItem, true))
                                        root._recordingRow = rowItem.index;
                                }
                            }

                            RowLayout {
                                id: kbRow
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 16
                                anchors.rightMargin: 16
                                spacing: 12

                                ViciImage {
                                    source: rowItem.icon ? Img.builtin(rowItem.icon).withBackgroundTint("accent") : ""
                                    Layout.preferredWidth: 22
                                    Layout.preferredHeight: 22
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
                                    visible: !rowItem.isRecording && rowItem.shortcutTokens.length === 0
                                    text: "Record Shortcut"
                                    color: Theme.textPlaceholder
                                    font.pointSize: Theme.smallerFontSize
                                }
                            }
                        }

                        ViciDivider {
                            visible: rowItem.index < keybindRepeater.count - 1
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
