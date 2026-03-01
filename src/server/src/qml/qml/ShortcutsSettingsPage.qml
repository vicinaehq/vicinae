import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    readonly property var model: settings.keybindModel

    Component.onCompleted: searchInput.forceActiveFocus()

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            color: "transparent"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                Image {
                    source: "image://vicinae/builtin:magnifying-glass?fg=" + Theme.textMuted
                    sourceSize.width: 16
                    sourceSize.height: 16
                    Layout.preferredWidth: 16
                    Layout.preferredHeight: 16
                }

                TextInput {
                    id: searchInput
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    verticalAlignment: TextInput.AlignVCenter
                    font.pointSize: Theme.regularFontSize
                    color: Theme.foreground
                    clip: true
                    activeFocusOnTab: true

                    Text {
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        text: "Search shortcuts..."
                        color: Theme.textPlaceholder
                        font: searchInput.font
                        visible: !searchInput.text
                    }

                    onTextEdited: root.model.setFilter(text)

                    Keys.onUpPressed: root.model.moveUp()
                    Keys.onDownPressed: root.model.moveDown()
                    Keys.onPressed: (event) => {
                        const ctrl = event.modifiers & Qt.ControlModifier
                        const noOther = !(event.modifiers & ~(Qt.ControlModifier | Qt.KeypadModifier | Qt.GroupSwitchModifier))
                        if (!ctrl || !noOther) return

                        if ((Config.emacsMode && event.key === Qt.Key_P)
                            || (!Config.emacsMode && event.key === Qt.Key_K)) {
                            root.model.moveUp()
                            event.accepted = true
                        } else if ((Config.emacsMode && event.key === Qt.Key_N)
                                   || (!Config.emacsMode && event.key === Qt.Key_J)) {
                            root.model.moveDown()
                            event.accepted = true
                        }
                    }
                    Keys.onReturnPressed: {
                        if (!root.model.hasSelection) return
                        const item = keybindList.itemAtIndex(root.model.selectedRow)
                        if (!item) return
                        shortcutRecorder.show(item.shortcutCellItem)
                    }
                    Keys.onEnterPressed: Keys.returnPressed(event)
                    Keys.onEscapePressed: {
                        if (text !== "") { text = ""; root.model.setFilter("") }
                    }
                }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.04)

            RowLayout {
                anchors.fill: parent
                spacing: 0

                Text {
                    text: "Name"
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize
                    font.bold: true
                    leftPadding: 12
                    Layout.fillWidth: true
                }
                Text {
                    text: "Shortcut"
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize
                    font.bold: true
                    Layout.preferredWidth: 200
                }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

        ListView {
            id: keybindList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.model
            boundsBehavior: Flickable.StopAtBounds
            activeFocusOnTab: false

            ScrollBar.vertical: ViciScrollBar {
                policy: keybindList.contentHeight > keybindList.height
                        ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }

            Connections {
                target: root.model
                function onSelectedChanged() {
                    if (root.model.selectedRow >= 0)
                        keybindList.positionViewAtIndex(root.model.selectedRow, ListView.Contain)
                }
            }

            delegate: Item {
                id: rowItem
                width: keybindList.width
                height: 38

                required property int index
                required property string name
                required property string icon
                required property string shortcut

                readonly property bool isSelected: index === root.model.selectedRow
                property alias shortcutCellItem: shortcutCell

                Rectangle {
                    anchors.fill: parent
                    color: Qt.rgba(Theme.foreground.r, Theme.foreground.g,
                                   Theme.foreground.b, 0.03)
                    visible: rowItem.index % 2 === 1 && !rowItem.isSelected && !rowHover.hovered
                }

                Rectangle {
                    anchors.fill: parent
                    color: rowItem.isSelected ? Theme.listItemSelectionBg
                           : rowHover.hovered ? Theme.listItemHoverBg
                           : "transparent"
                    visible: rowItem.isSelected || rowHover.hovered
                }

                HoverHandler { id: rowHover }
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    onClicked: {
                        root.model.select(rowItem.index)
                        searchInput.forceActiveFocus()
                    }
                    onDoubleClicked: {
                        root.model.select(rowItem.index)
                        shortcutRecorder.show(shortcutCell)
                    }
                }

                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.leftMargin: 12
                        Layout.rightMargin: 8
                        spacing: 10

                        ViciImage {
                            source: rowItem.icon
                                ? Img.builtin(rowItem.icon).withBackgroundTint("orange")
                                : ""
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: 20
                        }

                        Text {
                            text: rowItem.name
                            color: rowItem.isSelected ? Theme.listItemSelectionFg : Theme.foreground
                            font.pointSize: Theme.regularFontSize
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                    }

                    Item {
                        id: shortcutCell
                        Layout.preferredWidth: 200
                        Layout.fillHeight: true

                        Rectangle {
                            visible: rowItem.shortcut !== ""
                            anchors.left: parent.left
                            anchors.leftMargin: 4
                            anchors.verticalCenter: parent.verticalCenter
                            width: shortcutText.implicitWidth + 16
                            height: 24
                            radius: 4
                            color: rowItem.isSelected ? Qt.rgba(1, 1, 1, 0.15)
                                   : Qt.rgba(Theme.secondaryBackground.r,
                                              Theme.secondaryBackground.g,
                                              Theme.secondaryBackground.b,
                                              Config.windowOpacity)
                            border.color: rowItem.isSelected ? Qt.rgba(1, 1, 1, 0.2)
                                                             : Theme.divider
                            border.width: 1

                            Text {
                                id: shortcutText
                                anchors.centerIn: parent
                                text: rowItem.shortcut
                                color: rowItem.isSelected ? Theme.listItemSelectionFg : Theme.foreground
                                font.pointSize: Theme.smallerFontSize
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    root.model.select(rowItem.index)
                                    shortcutRecorder.show(shortcutCell)
                                }
                            }
                        }

                        Text {
                            visible: rowItem.shortcut === ""
                            anchors.left: parent.left
                            anchors.leftMargin: 4
                            anchors.verticalCenter: parent.verticalCenter
                            text: "Record shortcut"
                            color: Theme.textMuted
                            font.pointSize: Theme.smallerFontSize

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    root.model.select(rowItem.index)
                                    shortcutRecorder.show(shortcutCell)
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    ShortcutRecorderField {
        id: shortcutRecorder
        parent: root
        validateShortcut: (key, modifiers) => root.model.validateShortcut(key, modifiers)
        shortcutDisplayProvider: (key, modifiers) => root.model.shortcutDisplayString(key, modifiers)
        onShortcutCaptured: (key, modifiers) => {
            root.model.setShortcut(root.model.selectedRow, key, modifiers)
        }
        onClosed: searchInput.forceActiveFocus()
    }
}
