import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    implicitHeight: 36
    Layout.fillWidth: true
    activeFocusOnTab: true

    property alias text: innerInput.text
    property string placeholder: ""
    property bool readOnly: false
    property bool hasError: false
    readonly property bool editing: innerInput.editing

    // [{iconSource, title, value}]
    property var completions: []

    property string triggerChar: "{"

    signal textEdited()
    signal accepted()

    function forceActiveFocus() { innerInput.forceActiveFocus() }
    function selectAll() { innerInput.selectAll() }

    onActiveFocusChanged: {
        if (activeFocus) innerInput.forceActiveFocus()
    }

    property int _highlightedIndex: -1
    property var _filteredItems: []

    on_HighlightedIndexChanged: {
        if (_highlightedIndex >= 0)
            completionList.positionViewAtIndex(_highlightedIndex, ListView.Contain)
    }

    function _findTriggerStart() {
        var pos = innerInput.cursorPosition - 1
        var txt = innerInput.text
        for (var i = pos; i >= 0; i--) {
            var c = txt.charAt(i)
            if (c === '}') return -1
            if (c === root.triggerChar) return i
        }
        return -1
    }

    function _updatePopup() {
        var triggerIdx = _findTriggerStart()
        if (triggerIdx < 0) {
            completionPopup.close()
            return
        }

        var txt = innerInput.text
        var afterTrigger = txt.substring(triggerIdx + 1, innerInput.cursorPosition)
        var query = afterTrigger.toLowerCase().trim()
        var filtered = []
        for (var i = 0; i < completions.length; i++) {
            var item = completions[i]
            if (query === "" || item.title.toLowerCase().indexOf(query) >= 0) {
                filtered.push(item)
            }
        }
        _filteredItems = filtered
        _highlightedIndex = filtered.length > 0 ? 0 : -1

        if (filtered.length > 0) {
            completionPopup.open()
        } else {
            completionPopup.close()
        }
    }

    function _acceptCompletion() {
        if (_highlightedIndex < 0 || _highlightedIndex >= _filteredItems.length) return

        var item = _filteredItems[_highlightedIndex]
        var triggerIdx = _findTriggerStart()
        if (triggerIdx < 0) return

        var txt = innerInput.text
        var before = txt.substring(0, triggerIdx)
        var placeholder = triggerChar + item.value + "}"

        var endIdx = innerInput.cursorPosition
        while (endIdx < txt.length && txt.charAt(endIdx) !== "}" && txt.charAt(endIdx) !== triggerChar)
            endIdx++
        if (endIdx < txt.length && txt.charAt(endIdx) === "}")
            endIdx++

        var after = txt.substring(endIdx)
        innerInput.text = before + placeholder + after
        completionPopup.close()
        root.textEdited()
    }

    FormTextInput {
        id: innerInput
        anchors.fill: parent
        placeholder: root.placeholder
        readOnly: root.readOnly
        hasError: root.hasError

        onTextEdited: {
            root.textEdited()
            root._updatePopup()
        }
        onAccepted: {
            if (completionPopup.visible) {
                root._acceptCompletion()
            } else {
                root.accepted()
            }
        }

        Keys.onUpPressed: (event) => {
            if (completionPopup.visible) {
                event.accepted = true
                if (root._highlightedIndex > 0) root._highlightedIndex--
            }
        }
        Keys.onDownPressed: (event) => {
            if (completionPopup.visible) {
                event.accepted = true
                if (root._highlightedIndex < root._filteredItems.length - 1)
                    root._highlightedIndex++
            }
        }
        Keys.onEscapePressed: (event) => {
            if (completionPopup.visible) {
                event.accepted = true
                completionPopup.close()
            }
        }
    }

    Popup {
        id: completionPopup
        parent: root
        x: 0
        y: root.height + 4
        width: Math.max(200, root.width)
        padding: 4
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Rectangle {
            radius: 8
            color: Theme.background
            border.color: Theme.divider
            border.width: 1
        }

        contentItem: ListView {
            id: completionList
            implicitHeight: Math.min(contentHeight, 200)
            model: root._filteredItems.length
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            delegate: Item {
                id: del
                width: completionList.width
                height: 30

                required property int index
                readonly property var _item: root._filteredItems[index]
                readonly property bool _isHighlighted: index === root._highlightedIndex

                Rectangle {
                    anchors.fill: parent
                    anchors.leftMargin: 2
                    anchors.rightMargin: 2
                    radius: 6
                    color: del._isHighlighted ? Theme.listItemSelectionBg
                           : itemMouse.containsMouse ? Theme.listItemHoverBg
                           : "transparent"
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8
                    spacing: 6

                    ViciImage {
                        visible: del._item && del._item.iconSource ? true : false
                        source: visible ? del._item.iconSource : ""
                        Layout.preferredWidth: 16
                        Layout.preferredHeight: 16
                    }

                    Text {
                        text: del._item ? del._item.title : ""
                        color: del._isHighlighted ? Theme.listItemSelectionFg : Theme.foreground
                        font.pointSize: Theme.smallerFontSize
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                }

                MouseArea {
                    id: itemMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        root._highlightedIndex = del.index
                        root._acceptCompletion()
                    }
                }
            }
        }
    }
}
