import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    readonly property var extModel: settings.extensionModel

    onVisibleChanged: if (visible) searchInput.forceActiveFocus()

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Left pane: search + column headers + list (60%)
        ColumnLayout {
            Layout.fillHeight: true
            Layout.preferredWidth: root.width * 0.60
            Layout.maximumWidth: root.width * 0.60
            spacing: 0

            // Search bar
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
                            text: "Search extensions..."
                            color: Theme.textPlaceholder
                            font: searchInput.font
                            visible: !searchInput.text
                        }

                        onTextEdited: root.extModel.setFilter(text)

                        Keys.onUpPressed: root.extModel.moveUp()
                        Keys.onDownPressed: root.extModel.moveDown()
                        Keys.onReturnPressed: root.extModel.activate()
                        Keys.onEnterPressed: root.extModel.activate()
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
                activeFocusOnTab: false

                ScrollBar.vertical: ScrollBar {
                    policy: extList.contentHeight > extList.height
                            ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
                }

                Connections {
                    target: root.extModel
                    function onSelectedChanged() {
                        if (root.extModel.selectedRow >= 0)
                            extList.positionViewAtIndex(root.extModel.selectedRow, ListView.Contain)
                    }
                }

                delegate: Item {
                    id: rowItem
                    width: extList.width
                    height: 38

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

                    HoverHandler { id: rowHover }
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        onClicked: {
                            root.extModel.select(rowItem.index)
                            searchInput.forceActiveFocus()
                        }
                        onDoubleClicked: {
                            if (rowItem.expandable)
                                root.extModel.toggleExpanded(rowItem.index)
                        }
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
                            spacing: 10

                            // Expand indicator for providers
                            Item {
                                Layout.preferredWidth: 20
                                Layout.preferredHeight: 20
                                visible: rowItem.expandable

                                ViciImage {
                                    anchors.centerIn: parent
                                    source: Img.builtin(rowItem.expanded ? "chevron-down-small" : "chevron-right-small")
                                        .withFillColor(rowItem.isSelected ? Theme.listItemSelectionFg : Theme.textMuted)
                                    width: 20; height: 20
                                    sourceSize.width: 20; sourceSize.height: 20
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        root.extModel.toggleExpanded(rowItem.index)
                                        searchInput.forceActiveFocus()
                                    }
                                }
                            }

                            // Spacer for non-expandable items at indent > 0
                            Item {
                                Layout.preferredWidth: 20
                                visible: !rowItem.expandable && rowItem.indent > 0
                            }

                            ViciImage {
                                source: rowItem.iconSource
                                Layout.preferredWidth: 20
                                Layout.preferredHeight: 20
                            }

                            Text {
                                text: rowItem.name
                                color: rowItem.isSelected ? Theme.listItemSelectionFg
                                       : !rowItem.enabled ? Theme.textMuted
                                       : Theme.foreground
                                font.pointSize: Theme.regularFontSize
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
                            visible: !rowItem.isProvider

                            Rectangle {
                                anchors.fill: parent
                                anchors.margins: 4
                                radius: 4
                                color: "transparent"
                                border.color: aliasInput.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder
                                border.width: 1

                                TextInput {
                                    id: aliasInput
                                    anchors.fill: parent
                                    anchors.leftMargin: 6
                                    anchors.rightMargin: 6
                                    verticalAlignment: TextInput.AlignVCenter
                                    text: rowItem.alias
                                    color: rowItem.isSelected ? Theme.listItemSelectionFg : Theme.foreground
                                    font.pointSize: Theme.smallerFontSize
                                    clip: true

                                    Text {
                                        anchors.fill: parent
                                        verticalAlignment: Text.AlignVCenter
                                        text: "Add alias"
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
                            }
                        }

                        // Empty spacer for providers (no alias)
                        Item {
                            Layout.preferredWidth: 100
                            Layout.fillHeight: true
                            visible: rowItem.isProvider
                        }

                        // Enabled checkbox column
                        Item {
                            Layout.preferredWidth: 60
                            Layout.fillHeight: true

                            Rectangle {
                                anchors.centerIn: parent
                                width: 20
                                height: 20
                                radius: 4
                                color: rowItem.enabled ? Theme.accent : "transparent"
                                border.color: rowItem.enabled ? Theme.accent : Theme.inputBorder
                                border.width: 1

                                Text {
                                    anchors.centerIn: parent
                                    text: "\u2713"
                                    color: "#ffffff"
                                    font.pixelSize: 13
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

                }
            }
        }

        Rectangle { Layout.fillHeight: true; width: 1; color: Theme.divider }

        // Right pane: detail (40%)
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

                // Header — height matches left pane header (search 48 + divider 1 + columns 30 + divider 1 = 80)
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 80
                    Layout.leftMargin: 20
                    Layout.rightMargin: 20
                    spacing: 10

                    ViciImage {
                        source: root.extModel.selectedIconSource
                        Layout.preferredWidth: 30
                        Layout.preferredHeight: 30
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

                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

                // Scrollable detail content
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
                        spacing: 0

                        // Description (when non-empty)
                        ColumnLayout {
                            visible: root.extModel.selectedDescription !== ""
                            Layout.fillWidth: true
                            Layout.leftMargin: 20
                            Layout.rightMargin: 20
                            Layout.topMargin: 12
                            spacing: 4

                            Text {
                                text: "Description"
                                color: Theme.textMuted
                                font.pointSize: Theme.smallerFontSize
                                font.bold: true
                            }

                            Text {
                                text: root.extModel.selectedDescription
                                color: Theme.foreground
                                font.pointSize: Theme.regularFontSize
                                wrapMode: Text.Wrap
                                Layout.fillWidth: true
                            }

                            Item { implicitHeight: 8 }
                        }

                        // Metadata rows (for items like apps, scripts)
                        ColumnLayout {
                            visible: root.extModel.selectedMetadata.length > 0
                            Layout.fillWidth: true
                            Layout.leftMargin: 20
                            Layout.rightMargin: 20
                            Layout.topMargin: root.extModel.selectedDescription === "" ? 12 : 0
                            spacing: 0

                            Rectangle {
                                visible: root.extModel.selectedDescription !== ""
                                Layout.fillWidth: true
                                height: 1
                                color: Theme.divider
                                Layout.bottomMargin: 5
                            }

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

                        // Preferences
                        ColumnLayout {
                            visible: root.extModel.hasPreferences
                            Layout.fillWidth: true
                            Layout.leftMargin: 20
                            Layout.rightMargin: 20
                            Layout.topMargin: root.extModel.selectedDescription === ""
                                              && root.extModel.selectedMetadata.length === 0 ? 12 : 0
                            spacing: 0

                            Rectangle {
                                Layout.fillWidth: true
                                height: 1
                                color: Theme.divider
                                visible: root.extModel.selectedDescription !== ""
                                         || root.extModel.selectedMetadata.length > 0
                            }

                            Item { implicitHeight: 16 }

                            SettingsPreferenceForm {
                                Layout.fillWidth: true
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
