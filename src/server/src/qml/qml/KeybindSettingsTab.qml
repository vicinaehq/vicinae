import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    readonly property var model: settings.keybindModel

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Left pane: search + column headers + list
        ColumnLayout {
            Layout.fillHeight: true
            Layout.preferredWidth: root.width * 0.55
            spacing: 0

            // Search bar
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 44
                color: "transparent"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 6

                    Image {
                        source: "image://vicinae/builtin:magnifying-glass?fg=" + Theme.textMuted
                        sourceSize.width: 14
                        sourceSize.height: 14
                        Layout.preferredWidth: 14
                        Layout.preferredHeight: 14
                        opacity: 0.7
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
                            text: "Search keybinds..."
                            color: Theme.textPlaceholder
                            font: searchInput.font
                            visible: !searchInput.text
                        }

                        onTextEdited: root.model.setFilter(text)

                        Keys.onEscapePressed: {
                            if (text !== "") { text = ""; root.model.setFilter("") }
                        }
                    }
                }
            }

            Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

            // Column headers
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 28
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
                        Layout.preferredWidth: 180
                    }
                }
            }

            Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

            // List view
            ListView {
                id: keybindList
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: root.model
                boundsBehavior: Flickable.StopAtBounds

                ScrollBar.vertical: ScrollBar {
                    policy: keybindList.contentHeight > keybindList.height
                            ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
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

                    // Alternating row stripe
                    Rectangle {
                        anchors.fill: parent
                        color: Qt.rgba(Theme.foreground.r, Theme.foreground.g,
                                       Theme.foreground.b, 0.03)
                        visible: rowItem.index % 2 === 1 && !rowItem.isSelected && !rowHover.hovered
                    }

                    // Selection / hover highlight
                    Rectangle {
                        anchors.fill: parent
                        color: rowItem.isSelected ? Theme.listItemSelectionBg
                               : rowHover.hovered ? Theme.listItemHoverBg
                               : "transparent"
                        visible: rowItem.isSelected || rowHover.hovered
                    }

                    RowLayout {
                        anchors.fill: parent
                        spacing: 0

                        // Name column
                        RowLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.leftMargin: 12
                            Layout.rightMargin: 8
                            spacing: 8

                            ViciImage {
                                source: rowItem.icon ? Img.builtin(rowItem.icon) : ""
                                Layout.preferredWidth: 18
                                Layout.preferredHeight: 18
                            }

                            Text {
                                text: rowItem.name
                                color: rowItem.isSelected ? Theme.listItemSelectionFg : Theme.foreground
                                font.pointSize: Theme.regularFontSize
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                        }

                        // Shortcut column
                        Item {
                            Layout.preferredWidth: 180
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
                                                         : Theme.secondaryBackground
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
                            }

                            Text {
                                visible: rowItem.shortcut === ""
                                anchors.left: parent.left
                                anchors.leftMargin: 4
                                anchors.verticalCenter: parent.verticalCenter
                                text: "\u2014"
                                color: Theme.textMuted
                                font.pointSize: Theme.smallerFontSize
                            }
                        }
                    }

                    HoverHandler { id: rowHover }
                    TapHandler {
                        gesturePolicy: TapHandler.ReleaseWithinBounds
                        onTapped: root.model.select(rowItem.index)
                    }
                }
            }
        }

        Rectangle { Layout.fillHeight: true; width: 1; color: Theme.divider }

        // Right pane: detail
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Text {
                anchors.centerIn: parent
                visible: !root.model.hasSelection
                text: "Select a keybind to view details"
                color: Theme.textMuted
                font.pointSize: Theme.regularFontSize
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: 0
                visible: root.model.hasSelection

                // Header
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 52
                    Layout.leftMargin: 16
                    Layout.rightMargin: 16
                    spacing: 10

                    ViciImage {
                        source: root.model.selectedIcon ? Img.builtin(root.model.selectedIcon) : ""
                        Layout.preferredWidth: 28
                        Layout.preferredHeight: 28
                    }

                    Text {
                        text: root.model.selectedName
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize
                        font.bold: true
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

                // Description + shortcut recorder
                Flickable {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    contentWidth: width
                    contentHeight: detailCol.implicitHeight
                    clip: true
                    boundsBehavior: Flickable.StopAtBounds

                    ScrollBar.vertical: ScrollBar {
                        policy: parent.contentHeight > parent.height
                                ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
                    }

                    ColumnLayout {
                        id: detailCol
                        width: parent.width
                        spacing: 16

                        Item { implicitHeight: 4 }

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.leftMargin: 16
                            Layout.rightMargin: 16
                            spacing: 4

                            Text {
                                text: "Description"
                                color: Theme.textMuted
                                font.pointSize: Theme.smallerFontSize
                            }

                            Text {
                                text: root.model.selectedDescription
                                color: Theme.foreground
                                font.pointSize: Theme.regularFontSize
                                wrapMode: Text.Wrap
                                Layout.fillWidth: true
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            height: 1
                            color: Theme.divider
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.leftMargin: 16
                            Layout.rightMargin: 16
                            spacing: 4

                            Text {
                                text: "Shortcut"
                                color: Theme.textMuted
                                font.pointSize: Theme.smallerFontSize
                            }

                            ShortcutRecorderField {
                                Layout.fillWidth: true
                                Layout.maximumWidth: 300
                                onShortcutCaptured: (key, modifiers) => {
                                    root.model.setShortcut(root.model.selectedRow, key, modifiers)
                                }
                                validateShortcut: (key, modifiers) => root.model.validateShortcut(key, modifiers)
                            }
                        }

                        Item { implicitHeight: 16 }
                    }
                }
            }
        }
    }
}
