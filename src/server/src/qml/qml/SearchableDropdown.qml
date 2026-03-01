import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    implicitHeight: compact ? 28 : 36
    Layout.fillWidth: !compact
    activeFocusOnTab: !compact && !readOnly

    property var items: []
    property var currentItem: null
    signal activated(var item)
    signal popupClosed()

    property bool compact: false
    property real minimumWidth: 0

    property string placeholder: ""
    property bool readOnly: false
    property bool hasError: false

    width: compact ? Math.max(triggerButton.implicitWidth, minimumWidth) : implicitWidth

    onItemsChanged: _rebuildFlat("")

    property int _highlightedIndex: -1
    property real _closedTime: 0
    property var _flatItems: []

    on_HighlightedIndexChanged: {
        if (_highlightedIndex >= 0)
            itemList.positionViewAtIndex(_highlightedIndex, ListView.Contain)
    }

    function open() {
        if (root.readOnly) return
        if (dropdownPopup.visible) return
        if (searchField.text !== "") {
            searchField.text = ""
            _rebuildFlat("")
        }
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
        if (lower.indexOf(query) >= 0) return true
        var qi = 0
        for (var ti = 0; ti < lower.length && qi < query.length; ti++) {
            if (lower[ti] === query[qi]) qi++
        }
        return qi === query.length
    }

    function _nextNavigable(from, direction) {
        if (_flatItems.length === 0) return from
        let idx = from + direction
        if (idx < 0) idx = _flatItems.length - 1
        else if (idx >= _flatItems.length) idx = 0
        while (idx !== from) {
            if (!_flatItems[idx].isSection) return idx
            idx += direction
            if (idx < 0) idx = _flatItems.length - 1
            else if (idx >= _flatItems.length) idx = 0
        }
        return from
    }

    Keys.onReturnPressed: (event) => {
        if (event.modifiers !== Qt.NoModifier && typeof launcher !== "undefined") {
            event.accepted = launcher.forwardKey(event.key, event.modifiers)
        } else if (!dropdownPopup.visible) {
            open()
        }
    }
    Keys.onSpacePressed: {
        if (!dropdownPopup.visible) open()
    }
    Keys.onPressed: (event) => {
        if (typeof launcher !== "undefined")
            event.accepted = launcher.forwardKey(event.key, event.modifiers)
    }

    Rectangle {
        id: triggerButton
        opacity: root.readOnly ? 0.5 : 1.0
        implicitWidth: buttonRow.implicitWidth + 20
        anchors.fill: compact ? null : parent
        width: compact ? root.width : implicitWidth
        height: compact ? 28 : implicitHeight
        radius: compact ? 6 : 8
        color: buttonMouseArea.containsMouse ? Theme.listItemHoverBg
               : Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g,
                          Theme.secondaryBackground.b, Config.windowOpacity)
        border.color: compact ? Theme.divider
                      : root.hasError ? Theme.inputBorderError
                      : (root.activeFocus || dropdownPopup.visible
                         ? Theme.inputBorderFocus : Theme.inputBorder)
        border.width: 1

        RowLayout {
            id: buttonRow
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 6

            ViciImage {
                visible: root.currentItem && root.currentItem.iconSource
                         ? true : false
                source: visible ? root.currentItem.iconSource : ""
                Layout.preferredWidth: 16
                Layout.preferredHeight: 16
            }

            Text {
                text: root.currentItem ? root.currentItem.displayName : root.placeholder
                color: !compact && !root.currentItem ? Theme.textPlaceholder : Theme.foreground
                font.pointSize: compact ? Theme.smallerFontSize : Theme.regularFontSize
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
            cursorShape: root.readOnly ? Qt.ArrowCursor : Qt.PointingHandCursor
            onClicked: {
                if (Date.now() - root._closedTime < 300) return
                root.open()
            }
        }
    }

    Popup {
        id: dropdownPopup
        parent: triggerButton
        popupType: Popup.Window
        x: compact ? triggerButton.width - width : 0
        y: triggerButton.height + 4
        width: Math.max(compact ? 200 : 250, root.width)
        padding: 4
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        onOpened: searchField.forceActiveFocus()
        onActiveFocusChanged: if (!activeFocus && visible) close()
        onClosed: {
            root._closedTime = Date.now()
            root.popupClosed()
        }

        background: Rectangle {
            radius: 8
            color: Theme.background
            border.color: Theme.divider
            border.width: 1
        }

        contentItem: ColumnLayout {
            spacing: 0

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

            ListView {
                id: itemList
                Layout.fillWidth: true
                Layout.preferredHeight: Math.min(contentHeight, 300)
                model: root._flatItems.length
                clip: true
                reuseItems: true
                boundsBehavior: Flickable.StopAtBounds

                ScrollBar.vertical: ViciScrollBar {
                    policy: itemList.contentHeight > itemList.height
                            ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
                }

                delegate: Item {
                    id: del
                    width: itemList.width
                    height: !_entry ? 0 : _entry.isSection ? sectionRow.height : itemRow.height

                    required property int index
                    readonly property var _entry: index < root._flatItems.length
                                                  ? root._flatItems[index] : null
                    readonly property bool _isHighlighted: index === root._highlightedIndex
                    readonly property bool _isSelected: _entry && !_entry.isSection && root.currentItem
                                                        && _entry.item.id === root.currentItem.id

                    RowLayout {
                        id: sectionRow
                        visible: del._entry && del._entry.isSection
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        height: visible ? 24 : 0

                        Text {
                            text: del._entry && del._entry.isSection ? del._entry.title : ""
                            color: Theme.textMuted
                            font.pointSize: Theme.smallerFontSize
                            font.bold: true
                        }
                    }

                    Item {
                        id: itemRow
                        visible: del._entry && !del._entry.isSection
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: visible ? 30 : 0

                        Rectangle {
                            anchors.fill: parent
                            anchors.leftMargin: 2
                            anchors.rightMargin: 2
                            radius: 6
                            color: del._isHighlighted ? Theme.listItemSelectionBg
                                   : itemHover.hovered ? Theme.listItemHoverBg
                                   : "transparent"
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            anchors.rightMargin: 8
                            spacing: 6

                            ViciImage {
                                visible: del._entry && del._entry.item && del._entry.item.iconSource
                                         ? true : false
                                source: visible ? del._entry.item.iconSource : ""
                                Layout.preferredWidth: 16
                                Layout.preferredHeight: 16
                            }

                            Text {
                                text: del._entry && !del._entry.isSection
                                      ? del._entry.item.displayName : ""
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

                        HoverHandler {
                            id: itemHover
                            cursorShape: Qt.PointingHandCursor
                        }

                        TapHandler {
                            gesturePolicy: TapHandler.ReleaseWithinBounds
                            onTapped: {
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
