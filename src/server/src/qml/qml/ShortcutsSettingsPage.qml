import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    readonly property var model: settings.keybindModel

    property int _recordingRow: -1
    property string _recordingDisplay: ""
    property string _recordingStatus: ""
    property color _recordingStatusColor: Theme.textMuted

    function _startRecording(row) {
        _recordingRow = row
        _recordingDisplay = ""
        _recordingStatus = "Press a shortcut..."
        _recordingStatusColor = Theme.textMuted
        root.forceActiveFocus()
    }

    function _cancelRecording() {
        _recordingRow = -1
        _recordingDisplay = ""
        _recordingStatus = ""
    }

    Keys.onPressed: (event) => {
        if (root._recordingRow < 0) return

        event.accepted = true

        const key = event.key
        const mods = event.modifiers

        const isModKey = key === Qt.Key_Shift || key === Qt.Key_Control
                         || key === Qt.Key_Alt || key === Qt.Key_Meta
        const isCloseKey = key === Qt.Key_Escape || key === Qt.Key_Backspace

        if (!isModKey && isCloseKey && mods === Qt.NoModifier) {
            root._cancelRecording()
            return
        }

        root._recordingDisplay = root.model.shortcutDisplayString(key, mods)

        if (isModKey) {
            root._recordingStatus = "Press a key..."
            root._recordingStatusColor = Theme.textMuted
            return
        }

        const error = root.model.validateShortcut(key, mods)
        if (error !== "") {
            root._recordingStatus = error
            root._recordingStatusColor = Theme.danger
            return
        }

        root.model.setShortcut(root._recordingRow, key, mods)
        root._cancelRecording()
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
                border.color: searchField.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder
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

                        onTextChanged: root.model.setFilter(text)
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

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: Theme.divider
        }

        ListView {
            id: keybindList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.model
            boundsBehavior: Flickable.StopAtBounds

            ScrollBar.vertical: ViciScrollBar {
                policy: keybindList.contentHeight > keybindList.height
                        ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }

            delegate: Item {
                id: rowItem
                width: keybindList.width
                height: 45

                required property int index
                required property string name
                required property string icon
                required property string shortcut

                readonly property bool isRecording: index === root._recordingRow

                Rectangle {
                    anchors.fill: parent
                    anchors.bottomMargin: 1
                    color: rowItem.isRecording
                           ? Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.08)
                           : rowHover.hovered ? Theme.listItemHoverBg : "transparent"
                }

                HoverHandler { id: rowHover }
                MouseArea {
                    anchors.fill: parent
                    onClicked: root._startRecording(rowItem.index)
                }

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 12

                    ViciImage {
                        source: rowItem.icon
                            ? Img.builtin(rowItem.icon).withBackgroundTint("orange")
                            : ""
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
                        visible: rowItem.isRecording
                                 ? root._recordingDisplay !== ""
                                 : rowItem.shortcut !== ""
                        text: rowItem.isRecording
                              ? root._recordingDisplay
                              : rowItem.shortcut
                    }

                    Text {
                        visible: rowItem.isRecording
                        text: root._recordingStatus
                        color: root._recordingStatusColor
                        font.pointSize: Theme.smallerFontSize
                    }

                    Text {
                        visible: !rowItem.isRecording
                                 && rowItem.shortcut === ""
                                 && rowHover.hovered
                        text: "Record Shortcut"
                        color: Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                    }
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    height: 1
                    color: Theme.divider
                }
            }
        }
    }
}
