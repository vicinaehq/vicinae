import QtQuick
import QtQuick.Controls

Item {
    id: root

    // The backing model — must be a QAbstractListModel with roles:
    //   isSection, sectionName, rowSectionIdx, rowStartItem, rowItemCount
    // AND expose Q_INVOKABLEs for selection/navigation:
    //   selectedSection, selectedItem, select(), activateSelected(),
    //   navigateUp/Down/Left/Right(), navigateSectionUp/Down(),
    //   flatRowForSelection()
    property var cmdModel: null

    // Cell delegate component — instantiated per cell.
    // The Loader parent exposes context properties:
    //   cellSection (int), cellItem (int), cellSelected (bool),
    //   cellHovered (bool), cellSize (real), cellWidth (real),
    //   cellHeight (real), cmdModel (var)
    property Component cellDelegate: null

    property int columns: 8
    property real aspectRatio: 1.0
    property real cellSpacing: 10
    property real horizontalPadding: 20

    // Optional title/subtitle below each cell.
    // When enabled, the model must provide Q_INVOKABLE cellTitle(section, item)
    // and/or cellSubtitle(section, item).
    property bool showCellTitle: false
    property bool showCellSubtitle: false

    property string emptyTitle: "No results"
    property string emptyDescription: ""
    property var emptyIcon: Img.builtin("magnifying-glass").withFillColor(Theme.foreground)
    property Component emptyViewComponent: null

    property bool suppressEmpty: false

    // Infinite-scroll pagination: consumers opt in by setting canLoadMore.
    // endReached fires at most once per content growth cycle.
    signal endReached
    property bool canLoadMore: false
    property real endReachedThreshold: root.height * 1.5
    property bool _endArmed: true

    onCanLoadMoreChanged: {
        if (canLoadMore) {
            _endArmed = true;
            Qt.callLater(_maybeFireEnd);
        }
    }

    function _maybeFireEnd() {
        if (!root.canLoadMore || !root._endArmed)
            return;
        if (listView.contentHeight <= 0)
            return;
        const underfilled = listView.contentHeight <= listView.height;
        if (!underfilled && listView.atYBeginning)
            return;
        if (listView.contentY + listView.height >= listView.contentHeight - root.endReachedThreshold) {
            root._endArmed = false;
            root.endReached();
        }
    }

    readonly property bool _empty: listView.count === 0
    readonly property bool _awaitingData: root.cmdModel && root.cmdModel.awaitingData === true

    readonly property real cellSize: Math.floor((root.width - horizontalPadding * 2 - cellSpacing * (columns - 1)) / columns)

    HoverResetOnModelChange {
        target: root.cmdModel
    }

    // Hidden TextMetrics to measure actual line heights from the font
    TextMetrics {
        id: _titleMetrics
        font.pointSize: Theme.smallerFontSize
        text: "Ag"
    }
    TextMetrics {
        id: _subtitleMetrics
        font.pointSize: Theme.smallerFontSize - 1
        text: "Ag"
    }

    readonly property real _textGap: 6
    readonly property real cellTextHeight: {
        if (!showCellTitle && !showCellSubtitle)
            return 0;
        var h = _textGap;
        if (showCellTitle)
            h += _titleMetrics.height;
        if (showCellSubtitle)
            h += _subtitleMetrics.height;
        return h;
    }
    readonly property real rowHeight: cellSize + cellTextHeight

    function moveUp() {
        if (cmdModel)
            cmdModel.navigateUp();
        return true;
    }
    function moveDown() {
        if (cmdModel)
            cmdModel.navigateDown();
        return true;
    }
    function moveLeft() {
        if (cmdModel)
            cmdModel.navigateLeft();
        return true;
    }
    function moveRight() {
        if (cmdModel)
            cmdModel.navigateRight();
        return true;
    }
    function moveSectionUp() {
        if (cmdModel)
            cmdModel.navigateSectionUp();
        return true;
    }
    function moveSectionDown() {
        if (cmdModel)
            cmdModel.navigateSectionDown();
        return true;
    }

    function _isRowVisible(row) {
        if (row < 0)
            return false;

        const item = listView.itemAtIndex(row);
        if (!item)
            return false;

        const viewportTop = listView.contentY;
        const viewportBottom = viewportTop + listView.height;
        const itemTop = item.y;
        const itemBottom = item.y + item.height;

        return itemBottom > viewportTop && itemTop < viewportBottom;
    }

    ListView {
        id: listView
        anchors.fill: parent
        visible: !root._empty
        model: root.cmdModel
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        topMargin: root.cellSpacing
        bottomMargin: root.cellSpacing
        spacing: root.cellSpacing
        reuseItems: true
        cacheBuffer: 200

        property real _lastContentHeight: 0

        onContentYChanged: root._maybeFireEnd()
        onContentHeightChanged: {
            if (contentHeight > _lastContentHeight)
                root._endArmed = true;
            _lastContentHeight = contentHeight;
            root._maybeFireEnd();
        }

        ViciWheelHandler {
            target: listView
        }

        ScrollBar.vertical: ViciScrollBar {
            policy: listView.contentHeight > listView.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
        }

        delegate: Loader {
            id: delegateLoader
            width: ListView.view.width

            required property int index
            required property bool isSection
            required property string sectionName
            required property int rowSectionIdx
            required property int rowStartItem
            required property int rowItemCount
            required property int rowColumns
            required property double rowAspectRatio
            required property double rowInset

            sourceComponent: isSection ? sectionComponent : rowComponent

            Component {
                id: sectionComponent
                SectionHeader {
                    width: delegateLoader.width
                    text: delegateLoader.sectionName
                    leftPadding: root.horizontalPadding
                }
            }

            Component {
                id: rowComponent
                Item {
                    id: rowItem
                    width: delegateLoader.width

                    readonly property int effectiveCols: delegateLoader.rowColumns
                    readonly property real effectiveAspectRatio: delegateLoader.rowAspectRatio
                    readonly property real effectiveInset: delegateLoader.rowInset
                    readonly property real cellWidth: Math.floor((root.width - root.horizontalPadding * 2 - root.cellSpacing * (effectiveCols - 1)) / effectiveCols)
                    readonly property real cellHeight: Math.floor(cellWidth / effectiveAspectRatio)

                    readonly property bool rowHasTitle: {
                        if (!root.showCellTitle || !root.cmdModel || typeof root.cmdModel.cellTitle !== "function")
                            return false;
                        var _rev = root.cmdModel.dataRevision;
                        for (var i = 0; i < delegateLoader.rowItemCount; i++) {
                            if (root.cmdModel.cellTitle(delegateLoader.rowSectionIdx, delegateLoader.rowStartItem + i) !== "")
                                return true;
                        }
                        return false;
                    }

                    readonly property bool rowHasSubtitle: {
                        if (!root.showCellSubtitle || !root.cmdModel || typeof root.cmdModel.cellSubtitle !== "function")
                            return false;
                        var _rev = root.cmdModel.dataRevision;
                        for (var i = 0; i < delegateLoader.rowItemCount; i++) {
                            if (root.cmdModel.cellSubtitle(delegateLoader.rowSectionIdx, delegateLoader.rowStartItem + i) !== "")
                                return true;
                        }
                        return false;
                    }

                    readonly property real cellTextHeight: {
                        if (!rowHasTitle && !rowHasSubtitle)
                            return 0;
                        var h = root._textGap;
                        if (rowHasTitle)
                            h += _titleMetrics.height;
                        if (rowHasSubtitle)
                            h += _subtitleMetrics.height;
                        return h;
                    }

                    height: cellHeight + cellTextHeight

                    Row {
                        x: root.horizontalPadding
                        spacing: root.cellSpacing

                        Repeater {
                            model: delegateLoader.rowItemCount

                            delegate: Item {
                                id: cellWrapper

                                required property int index

                                readonly property int cellSection: delegateLoader.rowSectionIdx
                                readonly property int cellItem: delegateLoader.rowStartItem + index
                                readonly property bool cellSelected: root.cmdModel && root.cmdModel.selectedSection === cellSection && root.cmdModel.selectedItem === cellItem
                                readonly property bool cellHovered: cellMouseArea.containsMouse && HoverActivation.active

                                width: rowItem.cellWidth
                                height: rowItem.cellHeight + rowItem.cellTextHeight

                                SourceBlendRect {
                                    id: cellBackground
                                    width: rowItem.cellWidth
                                    height: rowItem.cellHeight
                                    radius: 10
                                    backgroundColor: {
                                        var bg = Theme.background;
                                        return Qt.rgba(bg.r, bg.g, bg.b, Config.windowOpacity);
                                    }
                                    color: {
                                        var bg = Theme.gridItemBackground;
                                        return Qt.rgba(bg.r, bg.g, bg.b, Config.surfaceOpacity);
                                    }
                                }

                                Loader {
                                    x: rowItem.cellWidth * rowItem.effectiveInset
                                    y: rowItem.cellHeight * rowItem.effectiveInset
                                    width: rowItem.cellWidth * (1 - 2 * rowItem.effectiveInset)
                                    height: rowItem.cellHeight * (1 - 2 * rowItem.effectiveInset)
                                    sourceComponent: root.cellDelegate
                                    property int cellSection: cellWrapper.cellSection
                                    property int cellItem: cellWrapper.cellItem
                                    property bool cellSelected: cellWrapper.cellSelected
                                    property bool cellHovered: cellWrapper.cellHovered
                                    property real cellSize: rowItem.cellWidth
                                    property real cellWidth: rowItem.cellWidth
                                    property real cellHeight: rowItem.cellHeight
                                    property var cmdModel: root.cmdModel
                                }

                                SourceBlendRect {
                                    visible: rowItem.effectiveInset <= 0
                                    width: rowItem.cellWidth
                                    height: rowItem.cellHeight
                                    radius: 10
                                    cornerMask: true
                                    backgroundColor: {
                                        var bg = Theme.background;
                                        return Qt.rgba(bg.r, bg.g, bg.b, Config.windowOpacity);
                                    }
                                }

                                SourceBlendRect {
                                    width: rowItem.cellWidth
                                    height: rowItem.cellHeight
                                    radius: 10
                                    overlay: true
                                    borderWidth: (cellWrapper.cellSelected || cellWrapper.cellHovered) ? 2 : 0
                                    borderColor: cellWrapper.cellSelected ? Theme.gridItemSelectionOutline : Theme.gridItemHoverOutline
                                }

                                Text {
                                    visible: rowItem.rowHasTitle
                                    y: rowItem.cellHeight + root._textGap
                                    width: rowItem.cellWidth
                                    height: _titleMetrics.height
                                    text: {
                                        var _rev = root.cmdModel ? root.cmdModel.dataRevision : 0;
                                        return (root.cmdModel && typeof root.cmdModel.cellTitle === "function") ? root.cmdModel.cellTitle(cellWrapper.cellSection, cellWrapper.cellItem) : "";
                                    }
                                    color: Theme.textMuted
                                    font: _titleMetrics.font
                                    elide: Text.ElideRight
                                    maximumLineCount: 1
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                Text {
                                    visible: rowItem.rowHasSubtitle
                                    y: rowItem.cellHeight + root._textGap + (rowItem.rowHasTitle ? _titleMetrics.height + root._textGap : 0)
                                    width: rowItem.cellWidth
                                    height: _subtitleMetrics.height
                                    text: {
                                        var _rev = root.cmdModel ? root.cmdModel.dataRevision : 0;
                                        return (root.cmdModel && typeof root.cmdModel.cellSubtitle === "function") ? root.cmdModel.cellSubtitle(cellWrapper.cellSection, cellWrapper.cellItem) : "";
                                    }
                                    color: Theme.textMuted
                                    font: _subtitleMetrics.font
                                    elide: Text.ElideRight
                                    maximumLineCount: 1
                                    horizontalAlignment: Text.AlignHCenter
                                    opacity: 0.7
                                }

                                MouseArea {
                                    id: cellMouseArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: {
                                        if (root.cmdModel) {
                                            root.cmdModel.select(cellWrapper.cellSection, cellWrapper.cellItem);
                                            if (Config.activateOnSingleClick)
                                                root.cmdModel.activateSelected();
                                        }
                                    }
                                    onDoubleClicked: {
                                        if (root.cmdModel) {
                                            root.cmdModel.select(cellWrapper.cellSection, cellWrapper.cellItem);
                                            root.cmdModel.activateSelected();
                                        }
                                    }
                                }

                                ViciToolTip {
                                    readonly property string tooltipText: root.cmdModel ? root.cmdModel.cellTooltip(cellWrapper.cellSection, cellWrapper.cellItem) : ""
                                    visible: cellWrapper.cellHovered && tooltipText !== ""
                                    text: tooltipText
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: root.cmdModel
        function onModelReset() {
            root._endArmed = true;
            listView._lastContentHeight = 0;
            Qt.callLater(root._maybeFireEnd);
        }
        function onSelectionChanged() {
            var row = root.cmdModel ? root.cmdModel.flatRowForSelection() : -1;
            if (row >= 0) {
                var mode = ListView.Contain;
                if (root.cmdModel && typeof root.cmdModel.alignSelectionScrollToTop === "function" && root.cmdModel.alignSelectionScrollToTop() && !root._isRowVisible(row)) {
                    mode = ListView.Beginning;
                }
                listView.positionViewAtIndex(row, mode);
            }
        }
    }

    Loader {
        anchors.fill: parent
        active: root._empty && !root.suppressEmpty && !root._awaitingData
        visible: active
        sourceComponent: root.emptyViewComponent ? root.emptyViewComponent : defaultEmptyView
    }

    Component {
        id: defaultEmptyView
        EmptyView {
            title: root.emptyTitle
            description: root.emptyDescription
            icon: root.emptyIcon
        }
    }
}
