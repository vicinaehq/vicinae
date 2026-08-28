import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: root

    property var items: []
    property var sections: []
    property CompletionModel model: null
    readonly property CompletionModel _model: model ?? internalModel
    property bool showFilter: false
    property string filterPlaceholder: qsTr("Filter...")
    property string currentItemId: ""

    // When true, show as a non-activating native window (so the field driving the
    // completion keeps focus) where the platform supports it; in-scene otherwise.
    property bool nativePanel: false

    popupType: nativePanel && Platform.supports("nativePanels") ? Popup.Window : Popup.Item

    readonly property int count: _model.count
    readonly property bool hasSelection: _highlightedIndex >= 0

    onCountChanged: {
        if (_highlightedIndex >= count)
            _highlightedIndex = -1;
    }

    readonly property real _bgOpacity: popupType === Popup.Window ? Config.popupOpacity : 0
    readonly property real _fillOpacity: Config.popupSurfaceOpacity

    signal itemAccepted(var itemData)

    property int _highlightedIndex: -1

    on_HighlightedIndexChanged: {
        if (_highlightedIndex >= 0)
            completionList.positionViewAtIndex(_highlightedIndex, ListView.Contain);
    }

    padding: 4
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    HoverResetOnShow {
        target: root
    }

    onItemsChanged: if (items.length > 0)
        internalModel.setItems(items)
    onSectionsChanged: if (sections.length > 0)
        internalModel.setSections(sections)

    onOpened: {
        if (showFilter) {
            filterField.text = "";
            _model.setFilter("");
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
            const idx = _model.indexOfItemId(currentItemId);
            if (idx >= 0) {
                _highlightedIndex = idx;
                return;
            }
        }
        const first = _model.nextSelectableIndex(-1, 1);
        _highlightedIndex = first >= 0 ? first : -1;
    }

    function filter(query) {
        _model.setFilter(query);
        const first = _model.nextSelectableIndex(-1, 1);
        _highlightedIndex = first >= 0 ? first : -1;
    }

    function moveUp() {
        _highlightedIndex = _model.nextSelectableIndex(_highlightedIndex, -1);
    }

    function moveDown() {
        _highlightedIndex = _model.nextSelectableIndex(_highlightedIndex, 1);
    }

    function acceptHighlighted() {
        if (_highlightedIndex < 0)
            return;
        const data = _model.itemDataAt(_highlightedIndex);
        if (Object.keys(data).length > 0) {
            itemAccepted(data);
            close();
        }
    }

    CompletionModel {
        id: internalModel
    }

    HoverResetOnModelChange {
        target: root._model
    }

    background: PopoverBackground {
        popup: root

        Loader {
            active: root.nativePanel && Platform.supports("nativePanels")
            source: "qrc:/Vicinae/CompletionPanelMacOS.qml"
        }

        PopupMaterial {}
    }

    contentItem: ColumnLayout {
        spacing: 0

        Item {
            visible: root.showFilter
            Layout.fillWidth: true
            Layout.preferredHeight: 28
            Layout.bottomMargin: 4

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                spacing: 6

                ViciImage {
                    source: Img.builtin("magnifying-glass").withFillColor(Theme.textMuted)
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

                    Timer {
                        id: filterDebounce
                        interval: 16
                        onTriggered: root.filter(filterField.text)
                    }

                    onTextEdited: filterDebounce.restart()

                    Keys.onReturnPressed: root.acceptHighlighted()
                    Keys.onEscapePressed: root.close()
                    Keys.onTabPressed: event => {
                        event.accepted = true;
                    }
                    Keys.onBacktabPressed: event => {
                        event.accepted = true;
                    }

                    Keys.onPressed: function (event) {
                        const nav = Keyboard.matchNavigation(event.key, event.modifiers);
                        if (event.key === Qt.Key_Up || nav === 1) {
                            root.moveUp();
                            event.accepted = true;
                        } else if (event.key === Qt.Key_Down || nav === 2) {
                            root.moveDown();
                            event.accepted = true;
                        }
                    }
                }
            }
        }

        ListView {
            id: completionList
            Layout.fillWidth: true
            Layout.preferredHeight: Math.min(contentHeight, root.showFilter ? 300 : 200)
            model: root._model
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            ViciWheelHandler {
                target: completionList
            }

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

                    SourceBlendRect {
                        anchors.fill: parent
                        anchors.leftMargin: 2
                        anchors.rightMargin: 2
                        radius: 6
                        backgroundColor: Qt.rgba(Theme.popoverBackground.r, Theme.popoverBackground.g, Theme.popoverBackground.b, root._bgOpacity)
                        color: {
                            if (del._isHighlighted) {
                                var c = Theme.listItemSelectionBg;
                                return Qt.rgba(c.r, c.g, c.b, root._fillOpacity);
                            }
                            if (itemHover.hovered && HoverActivation.active) {
                                var h = Theme.listItemHoverBg;
                                return Qt.rgba(h.r, h.g, h.b, root._fillOpacity);
                            }
                            var bg = Theme.popoverBackground;
                            return Qt.rgba(bg.r, bg.g, bg.b, root._bgOpacity);
                        }
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
