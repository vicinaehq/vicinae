import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    readonly property var extModel: settings.extensionModel

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Left pane: search + column headers + list
        ColumnLayout {
            Layout.fillHeight: true
            Layout.preferredWidth: root.width * 0.45
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
                            text: "Search extensions..."
                            color: Theme.textPlaceholder
                            font: searchInput.font
                            visible: !searchInput.text
                        }

                        onTextEdited: root.extModel.setFilter(text)

                        Keys.onEscapePressed: {
                            if (text !== "") { text = ""; root.extModel.setFilter("") }
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
                        Layout.minimumWidth: 200
                    }
                    Text {
                        text: "Type"
                        color: Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                        font.bold: true
                        Layout.preferredWidth: 100
                    }
                    Text {
                        text: "Alias"
                        color: Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                        font.bold: true
                        Layout.preferredWidth: 100
                    }
                    Text {
                        text: "On"
                        color: Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        Layout.preferredWidth: 60
                    }
                }
            }

            Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

            // List view
            ListView {
                id: extList
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: root.extModel
                boundsBehavior: Flickable.StopAtBounds

                ScrollBar.vertical: ScrollBar {
                    policy: extList.contentHeight > extList.height
                            ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
                }

                delegate: Item {
                    id: rowItem
                    width: extList.width
                    height: 34

                    required property int index
                    required property string name
                    required property string type
                    required property string iconSource
                    required property bool isProvider
                    required property bool enabled
                    required property string alias
                    required property bool expanded
                    required property bool expandable
                    required property int indent

                    readonly property bool isSelected: index === root.extModel.selectedRow

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
                            Layout.minimumWidth: 200
                            Layout.leftMargin: 8 + rowItem.indent * 20
                            Layout.rightMargin: 4
                            spacing: 6

                            // Expand indicator for providers
                            Item {
                                Layout.preferredWidth: 14
                                Layout.preferredHeight: 14
                                visible: rowItem.expandable

                                Text {
                                    anchors.centerIn: parent
                                    text: rowItem.expanded ? "\u25BE" : "\u25B8"
                                    font.pixelSize: 10
                                    color: rowItem.isSelected ? Theme.listItemSelectionFg : Theme.textMuted
                                }

                                TapHandler {
                                    onTapped: root.extModel.toggleExpanded(rowItem.index)
                                }
                            }

                            // Spacer for non-expandable items at indent > 0
                            Item {
                                Layout.preferredWidth: 14
                                visible: !rowItem.expandable && rowItem.indent > 0
                            }

                            ViciImage {
                                source: rowItem.iconSource
                                Layout.preferredWidth: rowItem.isProvider ? 18 : 16
                                Layout.preferredHeight: rowItem.isProvider ? 18 : 16
                            }

                            Text {
                                text: rowItem.name
                                color: rowItem.isSelected ? Theme.listItemSelectionFg
                                       : !rowItem.enabled ? Theme.textMuted
                                       : Theme.foreground
                                font.bold: rowItem.isProvider
                                font.pointSize: rowItem.isProvider ? Theme.regularFontSize
                                                                   : Theme.smallerFontSize
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                        }

                        // Type column
                        Text {
                            Layout.preferredWidth: 100
                            Layout.fillHeight: true
                            leftPadding: 4
                            verticalAlignment: Text.AlignVCenter
                            text: rowItem.type
                            color: rowItem.isSelected ? Theme.listItemSelectionFg : Theme.textMuted
                            font.pointSize: Theme.smallerFontSize
                            elide: Text.ElideRight
                        }

                        // Alias column
                        Item {
                            Layout.preferredWidth: 100
                            Layout.fillHeight: true

                            TextInput {
                                anchors.fill: parent
                                anchors.leftMargin: 4
                                anchors.rightMargin: 4
                                verticalAlignment: TextInput.AlignVCenter
                                text: rowItem.alias
                                color: rowItem.isSelected ? Theme.listItemSelectionFg : Theme.foreground
                                font.pointSize: Theme.smallerFontSize
                                clip: true
                                visible: !rowItem.isProvider

                                Text {
                                    anchors.fill: parent
                                    verticalAlignment: Text.AlignVCenter
                                    text: "\u2014"
                                    color: Theme.textPlaceholder
                                    font: parent.font
                                    visible: !parent.text && !parent.activeFocus
                                }

                                onActiveFocusChanged: {
                                    if (!activeFocus)
                                        root.extModel.setAlias(rowItem.index, text)
                                }
                                onAccepted: root.extModel.setAlias(rowItem.index, text)
                            }

                            Text {
                                visible: rowItem.isProvider
                                anchors.centerIn: parent
                                text: "\u2014"
                                color: Theme.textMuted
                                font.pointSize: Theme.smallerFontSize
                            }
                        }

                        // Enabled checkbox column
                        Item {
                            Layout.preferredWidth: 60
                            Layout.fillHeight: true

                            Rectangle {
                                anchors.centerIn: parent
                                width: 16
                                height: 16
                                radius: 4
                                color: rowItem.enabled ? Theme.accent : "transparent"
                                border.color: rowItem.enabled ? Theme.accent : Theme.inputBorder
                                border.width: 1

                                Text {
                                    anchors.centerIn: parent
                                    text: "\u2713"
                                    color: "#ffffff"
                                    font.pixelSize: 11
                                    font.bold: true
                                    visible: rowItem.enabled
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: root.extModel.setEnabled(rowItem.index, !rowItem.enabled)
                                }
                            }
                        }
                    }

                    HoverHandler { id: rowHover }
                    TapHandler {
                        gesturePolicy: TapHandler.ReleaseWithinBounds
                        onTapped: root.extModel.select(rowItem.index)
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
                visible: !root.extModel.hasSelection
                text: "Select an extension to view details"
                color: Theme.textMuted
                font.pointSize: Theme.regularFontSize
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: 0
                visible: root.extModel.hasSelection

                // Header
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 52
                    Layout.leftMargin: 16
                    Layout.rightMargin: 16
                    spacing: 10

                    ViciImage {
                        source: root.extModel.selectedIconSource
                        Layout.preferredWidth: 28
                        Layout.preferredHeight: 28
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Text {
                            text: root.extModel.selectedTitle
                            color: Theme.foreground
                            font.pointSize: Theme.regularFontSize
                            font.bold: true
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }

                        Text {
                            visible: root.extModel.selectedIsProvider
                            text: "Provider"
                            color: Theme.textMuted
                            font.pointSize: Theme.smallerFontSize
                        }
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

                // Controls + preference form
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
                        spacing: 12

                        Item { implicitHeight: 4 }

                        // Enabled toggle
                        RowLayout {
                            Layout.fillWidth: true
                            Layout.leftMargin: 16
                            Layout.rightMargin: 16
                            spacing: 8

                            FormCheckbox {
                                label: "Enabled"
                                checked: root.extModel.selectedEnabled
                                onToggled: root.extModel.setEnabled(
                                    root.extModel.selectedRow, checked)
                            }
                        }

                        // Alias field (only for non-provider items)
                        ColumnLayout {
                            visible: !root.extModel.selectedIsProvider
                            Layout.fillWidth: true
                            Layout.leftMargin: 16
                            Layout.rightMargin: 16
                            spacing: 4

                            Text {
                                text: "Alias"
                                color: Theme.textMuted
                                font.pointSize: Theme.smallerFontSize
                            }

                            FormTextInput {
                                id: aliasInput
                                text: root.extModel.selectedAlias
                                placeholder: "Enter an alias..."
                                onAccepted: root.extModel.setAlias(
                                    root.extModel.selectedRow, text)
                                onActiveFocusChanged: {
                                    if (!activeFocus && root.extModel.hasSelection)
                                        root.extModel.setAlias(
                                            root.extModel.selectedRow, text)
                                }
                            }
                        }

                        // Preferences
                        ColumnLayout {
                            visible: root.extModel.hasPreferences
                            Layout.fillWidth: true
                            spacing: 4

                            Rectangle {
                                Layout.fillWidth: true
                                height: 1
                                color: Theme.divider
                                Layout.topMargin: 4
                            }

                            Text {
                                text: "Preferences"
                                color: Theme.textMuted
                                font.pointSize: Theme.smallerFontSize
                                Layout.leftMargin: 16
                                Layout.topMargin: 8
                            }

                            PreferenceFormView {
                                Layout.fillWidth: true
                                implicitHeight: childrenRect.height
                                prefModel: root.extModel.preferenceModel
                            }
                        }

                        Item { implicitHeight: 16 }
                    }
                }
            }
        }
    }
}
