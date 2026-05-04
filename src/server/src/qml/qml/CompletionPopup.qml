import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: root

    property var items: []
    property var sections: []
    property bool showFilter: false
    property string filterPlaceholder: "Filter..."
    property string currentItemId: ""

    readonly property int count: completionModel.count
    readonly property bool hasSelection: _highlightedIndex >= 0

    signal itemAccepted(var itemData)

    property int _highlightedIndex: -1

    on_HighlightedIndexChanged: {
        if (_highlightedIndex >= 0)
            completionList.positionViewAtIndex(_highlightedIndex, ListView.Contain);
    }

    padding: 4
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    onItemsChanged: if (items.length > 0)
        completionModel.setItems(items)
    onSectionsChanged: if (sections.length > 0)
        completionModel.setSections(sections)

    onOpened: {
        if (showFilter) {
            filterField.text = "";
            completionModel.setFilter("");
            _highlightCurrentOrFirst();
            filterField.forceActiveFocus();
        }
    }
    onActiveFocusChanged: {
        if (showFilter && !activeFocus && visible)
            close();
    }

    function _highlightCurrentOrFirst() {
        if (currentItemId !== "") {
            const idx = completionModel.indexOfItemId(currentItemId);
            if (idx >= 0) {
                _highlightedIndex = idx;
                return;
            }
        }
        const first = completionModel.nextSelectableIndex(-1, 1);
        _highlightedIndex = first >= 0 ? first : -1;
    }

    function filter(query) {
        completionModel.setFilter(query);
        const first = completionModel.nextSelectableIndex(-1, 1);
        _highlightedIndex = first >= 0 ? first : -1;
    }

    function moveUp() {
        _highlightedIndex = completionModel.nextSelectableIndex(_highlightedIndex, -1);
    }

    function moveDown() {
        _highlightedIndex = completionModel.nextSelectableIndex(_highlightedIndex, 1);
    }

    function acceptHighlighted() {
        if (_highlightedIndex < 0)
            return;
        const data = completionModel.itemDataAt(_highlightedIndex);
        if (Object.keys(data).length > 0) {
            itemAccepted(data);
            close();
        }
    }

    CompletionModel {
        id: completionModel
        onCountChanged: {
            if (root._highlightedIndex >= count)
                root._highlightedIndex = -1;
        }
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
            visible: root.showFilter
            Layout.fillWidth: true
            Layout.preferredHeight: 28
            Layout.bottomMargin: 4
            radius: 6
            color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g, Theme.secondaryBackground.b, Theme.surfaceOpacity)

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
                    id: filterField
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
                        text: root.filterPlaceholder
                        color: Theme.textPlaceholder
                        font: filterField.font
                        visible: !filterField.text
                    }

                    onTextEdited: root.filter(text)

                    Keys.onUpPressed: root.moveUp()
                    Keys.onDownPressed: root.moveDown()
                    Keys.onReturnPressed: root.acceptHighlighted()
                    Keys.onEscapePressed: root.close()
                    Keys.onTabPressed: event => {
                        event.accepted = true;
                    }
                    Keys.onBacktabPressed: event => {
                        event.accepted = true;
                    }
                }
            }
        }

        ListView {
            id: completionList
            Layout.fillWidth: true
            Layout.preferredHeight: Math.min(contentHeight, root.showFilter ? 300 : 200)
            model: completionModel
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            ScrollBar.vertical: ViciScrollBar {
                policy: completionList.contentHeight > completionList.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }

            delegate: Item {
                id: del
                width: completionList.width
                height: itemType === "section" ? sectionRow.height : itemRow.height

                required property int index
                required property string itemType
                required property string title
                required property string iconSource
                required property var itemData

                readonly property bool _isHighlighted: index === root._highlightedIndex
                readonly property bool _isSelected: itemType === "item" && root.currentItemId !== "" && itemData && itemData.id === root.currentItemId

                RowLayout {
                    id: sectionRow
                    visible: del.itemType === "section"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8
                    height: visible ? 24 : 0

                    Text {
                        text: del.title
                        color: Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                        font.bold: true
                    }
                }

                Item {
                    id: itemRow
                    visible: del.itemType === "item"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: visible ? 30 : 0

                    Rectangle {
                        anchors.fill: parent
                        anchors.leftMargin: 2
                        anchors.rightMargin: 2
                        radius: 6
                        color: del._isHighlighted ? Theme.listItemSelectionBg : itemHover.hovered ? Theme.listItemHoverBg : "transparent"
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        spacing: 6

                        ViciImage {
                            visible: del.iconSource !== ""
                            source: visible ? del.iconSource : ""
                            Layout.preferredWidth: 16
                            Layout.preferredHeight: 16
                        }

                        Text {
                            text: del.title
                            color: del._isHighlighted ? Theme.listItemSelectionFg : Theme.foreground
                            font.pointSize: Theme.smallerFontSize
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }

                        Text {
                            visible: del._isSelected
                            text: "✓"
                            color: del._isHighlighted ? Theme.listItemSelectionFg : Theme.foreground
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
                            root._highlightedIndex = del.index;
                            root.acceptHighlighted();
                        }
                    }
                }
            }
        }
    }
}
