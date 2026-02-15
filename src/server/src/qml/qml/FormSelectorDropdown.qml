import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    implicitHeight: 36
    Layout.fillWidth: true
    activeFocusOnTab: true

    // currentItem: { id, displayName, iconSource } or null
    property var currentItem: null
    // items: [{ title: "Section", items: [{ id, displayName, iconSource }, ...] }, ...]
    property var items: []
    property string placeholder: ""

    signal activated(var item)

    property int _highlightedIndex: -1
    property bool _closedByClick: false
    property var _flatItems: []

    function open() {
        if (dropdownPopup.visible) return
        _rebuildFlat("")
        searchField.text = ""
        _highlightedIndex = _flatIndexOfCurrent()
        dropdownPopup.open()
    }

    function _flatIndexOfCurrent() {
        if (!currentItem) return _nextNavigable(-1, 1)
        for (var i = 0; i < _flatItems.length; i++) {
            var entry = _flatItems[i]
            if (!entry.isSection && entry.item && entry.item.id === currentItem.id)
                return i
        }
        return _nextNavigable(-1, 1)
    }

    function _rebuildFlat(query) {
        var result = []
        var q = query.toLowerCase()
        for (var s = 0; s < items.length; s++) {
            var section = items[s]
            var sectionItems = []
            for (var i = 0; i < section.items.length; i++) {
                var item = section.items[i]
                if (q === "" || _fuzzyMatch(item.displayName, q)) {
                    sectionItems.push({ isSection: false, item: item })
                }
            }
            if (sectionItems.length > 0) {
                // Only show section header if there are multiple sections
                if (items.length > 1 && section.title) {
                    result.push({ isSection: true, title: section.title })
                }
                result = result.concat(sectionItems)
            }
        }
        _flatItems = result
    }

    function _fuzzyMatch(text, query) {
        var lower = text.toLowerCase()
        // Substring match first
        if (lower.indexOf(query) >= 0) return true
        // Character-by-character fuzzy match
        var qi = 0
        for (var ti = 0; ti < lower.length && qi < query.length; ti++) {
            if (lower[ti] === query[qi]) qi++
        }
        return qi === query.length
    }

    function _nextNavigable(from, direction) {
        var idx = from + direction
        while (idx >= 0 && idx < _flatItems.length) {
            if (!_flatItems[idx].isSection) return idx
            idx += direction
        }
        return from
    }

    onActiveFocusChanged: {
        if (activeFocus && !dropdownPopup.visible) open()
    }

    // Trigger button
    Rectangle {
        id: triggerButton
        anchors.fill: parent
        radius: 8
        color: buttonMouseArea.containsMouse ? Theme.listItemHoverBg : Theme.secondaryBackground
        border.color: root.activeFocus || dropdownPopup.visible
                      ? Theme.inputBorderFocus : Theme.inputBorder
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 6

            ViciImage {
                visible: root.currentItem && root.currentItem.iconSource
                         ? true : false
                source: visible ? root.currentItem.iconSource : Img.builtin("chevron-down")
                Layout.preferredWidth: 16
                Layout.preferredHeight: 16
            }

            Text {
                text: root.currentItem ? root.currentItem.displayName : root.placeholder
                color: root.currentItem ? Theme.foreground : Theme.textPlaceholder
                font.pointSize: Theme.regularFontSize
                elide: Text.ElideRight
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
                if (root._closedByClick) {
                    root._closedByClick = false
                    return
                }
                root.open()
            }
        }
    }

    Popup {
        id: dropdownPopup
        parent: triggerButton
        x: 0
        y: triggerButton.height + 4
        width: Math.max(250, root.width)
        padding: 4
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        onOpened: searchField.forceActiveFocus()
        onClosed: {
            if (buttonMouseArea.containsMouse)
                root._closedByClick = true
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
                            root._rebuildFlat(text)
                            root._highlightedIndex = root._nextNavigable(-1, 1)
                        }

                        Keys.onUpPressed: {
                            root._highlightedIndex = root._nextNavigable(root._highlightedIndex, -1)
                        }
                        Keys.onDownPressed: {
                            root._highlightedIndex = root._nextNavigable(root._highlightedIndex, 1)
                        }
                        Keys.onReturnPressed: {
                            if (root._highlightedIndex >= 0
                                    && root._highlightedIndex < root._flatItems.length) {
                                var entry = root._flatItems[root._highlightedIndex]
                                if (!entry.isSection) {
                                    root.activated(entry.item)
                                    dropdownPopup.close()
                                }
                            }
                        }
                        Keys.onEscapePressed: dropdownPopup.close()
                        Keys.onTabPressed: (event) => { event.accepted = true }
                        Keys.onBacktabPressed: (event) => { event.accepted = true }
                    }
                }
            }

            // Items list in a scrollable area
            ListView {
                id: itemList
                Layout.fillWidth: true
                Layout.preferredHeight: Math.min(contentHeight, 300)
                model: root._flatItems.length
                clip: true
                boundsBehavior: Flickable.StopAtBounds

                ScrollBar.vertical: ScrollBar {
                    policy: itemList.contentHeight > itemList.height
                            ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
                }

                delegate: Item {
                    id: del
                    width: itemList.width
                    height: _entry.isSection ? sectionRow.implicitHeight : itemRow.implicitHeight

                    required property int index
                    readonly property var _entry: root._flatItems[index]
                    readonly property bool _isHighlighted: index === root._highlightedIndex
                    readonly property bool _isSelected: !_entry.isSection && root.currentItem
                                                        && _entry.item.id === root.currentItem.id

                    // Section header
                    RowLayout {
                        id: sectionRow
                        visible: del._entry.isSection
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        height: visible ? 24 : 0

                        Text {
                            text: del._entry.isSection ? del._entry.title : ""
                            color: Theme.textMuted
                            font.pointSize: Theme.smallerFontSize
                            font.bold: true
                        }
                    }

                    // Item row
                    Item {
                        id: itemRow
                        visible: !del._entry.isSection
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: visible ? 30 : 0

                        Rectangle {
                            anchors.fill: parent
                            anchors.leftMargin: 2
                            anchors.rightMargin: 2
                            radius: 6
                            color: del._isHighlighted ? Theme.listItemSelectionBg
                                   : itemMouseArea.containsMouse ? Theme.listItemHoverBg
                                   : "transparent"
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            anchors.rightMargin: 8
                            spacing: 6

                            ViciImage {
                                visible: del._entry.item && del._entry.item.iconSource
                                         ? true : false
                                source: visible ? del._entry.item.iconSource
                                                : Img.builtin("chevron-down")
                                Layout.preferredWidth: 16
                                Layout.preferredHeight: 16
                            }

                            Text {
                                text: del._entry.isSection ? "" : del._entry.item.displayName
                                color: del._isHighlighted ? Theme.listItemSelectionFg
                                       : Theme.foreground
                                font.pointSize: Theme.smallerFontSize
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }

                            Text {
                                visible: del._isSelected
                                text: "\u2713"
                                color: del._isHighlighted ? Theme.listItemSelectionFg
                                       : Theme.foreground
                                font.pointSize: Theme.smallerFontSize
                            }
                        }

                        MouseArea {
                            id: itemMouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                root.activated(del._entry.item)
                                dropdownPopup.close()
                            }
                        }
                    }
                }
            }
        }
    }
}
