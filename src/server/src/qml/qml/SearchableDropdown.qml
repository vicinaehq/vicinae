import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    width: Math.max(triggerButton.implicitWidth, minimumWidth)
    height: 28
    activeFocusOnTab: false

    property var options: []
    property int currentIndex: 0
    property real minimumWidth: 0
    signal activated(int index)

    property int _highlightedIndex: -1
    property real _closedTime: 0

    function open() {
        if (dropdownPopup.visible) return
        searchField.text = ""
        root._highlightedIndex = root.currentIndex
        dropdownPopup.open()
    }

    function _isVisible(idx) {
        var query = searchField.text.toLowerCase()
        if (query === "") return true
        return root.options[idx].toLowerCase().indexOf(query) >= 0
    }

    function _nextVisible(from, direction) {
        var idx = from + direction
        while (idx >= 0 && idx < root.options.length) {
            if (_isVisible(idx)) return idx
            idx += direction
        }
        return from
    }

    // Trigger button
    Rectangle {
        id: triggerButton
        implicitWidth: buttonRow.implicitWidth + 20
        width: root.width
        height: 28
        radius: 6
        color: buttonMouseArea.containsMouse ? Theme.listItemHoverBg : Theme.secondaryBackground
        border.color: Theme.divider
        border.width: 1

        RowLayout {
            id: buttonRow
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 4

            Text {
                text: root.options[root.currentIndex] || ""
                color: Theme.foreground
                font.pointSize: Theme.smallerFontSize
                Layout.fillWidth: true
            }

            ViciImage {
                source: dropdownPopup.visible
                        ? Img.builtin("chevron-up") : Img.builtin("chevron-down")
                Layout.preferredWidth: 10
                Layout.preferredHeight: 10
            }
        }

        MouseArea {
            id: buttonMouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                if (Date.now() - root._closedTime < 300) return
                root.open()
            }
        }
    }

    Popup {
        id: dropdownPopup
        parent: triggerButton
        x: triggerButton.width - width
        y: triggerButton.height + 4
        width: Math.max(200, root.width)
        padding: 4
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        onOpened: searchField.forceActiveFocus()
        onClosed: {
            root._closedTime = Date.now()
        }

        background: Rectangle {
            radius: 8
            color: Theme.background
            border.color: Theme.divider
            border.width: 1
        }

        contentItem: ColumnLayout {
            spacing: 0

            // Search field
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 28
                Layout.bottomMargin: 4
                radius: 6
                color: Theme.secondaryBackground

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8
                    spacing: 6

                    Image {
                        source: "image://vicinae/builtin:magnifying-glass?fg=" + Theme.textMuted
                        sourceSize.width: 12
                        sourceSize.height: 12
                        Layout.preferredWidth: 12
                        Layout.preferredHeight: 12
                        opacity: 0.7
                    }

                    TextInput {
                        id: searchField
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        verticalAlignment: TextInput.AlignVCenter
                        font.pointSize: Theme.smallerFontSize
                        color: Theme.foreground
                        clip: true
                        activeFocusOnTab: false

                        Text {
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter
                            text: "Filter..."
                            color: Theme.textPlaceholder
                            font: searchField.font
                            visible: !searchField.text
                        }

                        onTextEdited: {
                            // Reset highlight to first visible option when filter changes
                            root._highlightedIndex = root._nextVisible(-1, 1)
                        }

                        Keys.onUpPressed: {
                            root._highlightedIndex = root._nextVisible(root._highlightedIndex, -1)
                        }
                        Keys.onDownPressed: {
                            root._highlightedIndex = root._nextVisible(root._highlightedIndex, 1)
                        }
                        Keys.onReturnPressed: {
                            if (root._highlightedIndex >= 0) {
                                root.activated(root._highlightedIndex)
                                dropdownPopup.close()
                            }
                        }
                        Keys.onEscapePressed: dropdownPopup.close()
                        Keys.onTabPressed: (event) => { event.accepted = true }
                        Keys.onBacktabPressed: (event) => { event.accepted = true }
                    }
                }
            }

            // Options
            Repeater {
                model: root.options

                delegate: Item {
                    id: optionDelegate
                    Layout.fillWidth: true
                    Layout.preferredHeight: optionVisible ? 30 : 0
                    visible: optionVisible

                    readonly property bool optionVisible: root._isVisible(index)
                    readonly property bool isHighlighted: index === root._highlightedIndex
                    readonly property bool isSelected: index === root.currentIndex
                    readonly property bool isHovered: optionMouseArea.containsMouse

                    Rectangle {
                        anchors.fill: parent
                        radius: 6
                        color: optionDelegate.isHighlighted ? Theme.listItemSelectionBg
                               : optionDelegate.isHovered ? Theme.listItemHoverBg
                               : "transparent"
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        spacing: 6

                        Text {
                            text: modelData
                            color: optionDelegate.isHighlighted
                                   ? Theme.listItemSelectionFg : Theme.foreground
                            font.pointSize: Theme.smallerFontSize
                            Layout.fillWidth: true
                        }

                        Text {
                            visible: optionDelegate.isSelected
                            text: "\u2713"
                            color: optionDelegate.isHighlighted
                                   ? Theme.listItemSelectionFg : Theme.foreground
                            font.pointSize: Theme.smallerFontSize
                        }
                    }

                    MouseArea {
                        id: optionMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            root.activated(index)
                            dropdownPopup.close()
                        }
                    }
                }
            }
        }
    }
}
