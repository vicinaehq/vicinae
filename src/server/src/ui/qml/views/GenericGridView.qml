pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

Item {
    id: root

    readonly property LauncherAppearance appearance: (root.Window.window as LauncherWindow)?.appearance ?? fallbackAppearance

    LauncherAppearance {
        id: fallbackAppearance
    }

    property SectionGridModel cmdModel: null

    // Cell delegate component, instantiated per cell inside a GridCell loader
    // that exposes the cell's data and geometry.
    property Component cellDelegate: null

    property int columns: 8
    property real aspectRatio: 1.0
    property real cellSpacing: 10
    property real horizontalPadding: 20

    property bool showCellTitle: false
    property bool showCellSubtitle: false

    property string emptyTitle: qsTr("No results")
    property string emptyDescription: ""
    property var emptyIcon: Img.icon(BuiltinIcon.MagnifyingGlass).withFillColor(Theme.foreground)
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
        const underfilled = !viewport.scrollable;
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
        id: titleMetrics
        font.pointSize: Theme.smallerFontSize
        text: "Ag"
    }
    TextMetrics {
        id: subtitleMetrics
        font.pointSize: Theme.smallerFontSize - 1
        text: "Ag"
    }

    readonly property real _textGap: 6
    readonly property real cellTextHeight: {
        if (!showCellTitle && !showCellSubtitle)
            return 0;
        var h = _textGap;
        if (showCellTitle)
            h += titleMetrics.height;
        if (showCellSubtitle)
            h += subtitleMetrics.height;
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

    ScrollViewport {
        id: viewport
        anchors.fill: parent
        visible: !root._empty
        flickable: listView
        topPadding: root.cellSpacing
        bottomPadding: root.cellSpacing

        ListView {
            id: listView
            anchors.fill: parent
            model: root.cmdModel
            clip: true
            interactive: false
            boundsBehavior: Flickable.StopAtBounds
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
                policy: viewport.scrollable ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
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
                required property var rowCells

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

                        readonly property bool rowHasTitle: root.showCellTitle && delegateLoader.rowCells.some(cell => cell.title !== "")
                        readonly property bool rowHasSubtitle: root.showCellSubtitle && delegateLoader.rowCells.some(cell => cell.subtitle !== "")

                        readonly property real cellTextHeight: {
                            if (!rowHasTitle && !rowHasSubtitle)
                                return 0;
                            var h = root._textGap;
                            if (rowHasTitle)
                                h += titleMetrics.height;
                            if (rowHasSubtitle)
                                h += subtitleMetrics.height;
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
                                    readonly property var cell: delegateLoader.rowCells[index]
                                    readonly property bool cellSelected: root.cmdModel && root.cmdModel.selectedSection === cellSection && root.cmdModel.selectedItem === cellItem
                                    readonly property bool cellHovered: cellMouseArea.containsMouse && HoverActivation.active

                                    Accessible.role: Accessible.Cell
                                    Accessible.name: root.showCellTitle ? (cellWrapper.cell?.title ?? "") : ""
                                    Accessible.description: root.showCellSubtitle ? (cellWrapper.cell?.subtitle ?? "") : ""
                                    Accessible.selectable: true
                                    Accessible.selected: cellWrapper.cellSelected

                                    width: rowItem.cellWidth
                                    height: rowItem.cellHeight + rowItem.cellTextHeight

                                    SourceBlendRect {
                                        id: cellBackground
                                        width: rowItem.cellWidth
                                        height: rowItem.cellHeight
                                        radius: 10
                                        backgroundColor: root.appearance.delegateBackdrop
                                        color: cellWrapper.cellSelected ? root.appearance.gridSelectionFill : cellWrapper.cellHovered ? root.appearance.gridHoverFill : root.appearance.gridFill
                                    }

                                    GridCell {
                                        x: rowItem.cellWidth * rowItem.effectiveInset
                                        y: rowItem.cellHeight * rowItem.effectiveInset
                                        width: rowItem.cellWidth * (1 - 2 * rowItem.effectiveInset)
                                        height: rowItem.cellHeight * (1 - 2 * rowItem.effectiveInset)
                                        sourceComponent: root.cellDelegate
                                        layer.enabled: rowItem.effectiveInset <= 0 && root.appearance.gridContentEffect !== null
                                        layer.effect: root.appearance.gridContentEffect
                                        cellSection: cellWrapper.cellSection
                                        cellItem: cellWrapper.cellItem
                                        cellSelected: cellWrapper.cellSelected
                                        cellHovered: cellWrapper.cellHovered
                                        cellSize: rowItem.cellWidth
                                        cellWidth: rowItem.cellWidth
                                        cellHeight: rowItem.cellHeight
                                        cell: cellWrapper.cell
                                        cmdModel: root.cmdModel
                                    }

                                    SourceBlendRect {
                                        visible: rowItem.effectiveInset <= 0 && root.appearance.gridContentEffect === null
                                        width: rowItem.cellWidth
                                        height: rowItem.cellHeight
                                        radius: 10
                                        cornerMask: true
                                        backgroundColor: {
                                            var bg = Theme.background;
                                            return Config.withAlpha(bg, Config.windowOpacity);
                                        }
                                    }

                                    SourceBlendRect {
                                        width: rowItem.cellWidth
                                        height: rowItem.cellHeight
                                        radius: 10
                                        overlay: true
                                        borderWidth: cellWrapper.cellSelected ? root.appearance.gridSelectionBorderWidth : cellWrapper.cellHovered ? root.appearance.gridHoverBorderWidth : 0
                                        borderColor: cellWrapper.cellSelected ? root.appearance.gridSelectionOutline : root.appearance.gridHoverOutline
                                    }

                                    Text {
                                        visible: rowItem.rowHasTitle
                                        y: rowItem.cellHeight + root._textGap
                                        width: rowItem.cellWidth
                                        height: titleMetrics.height
                                        text: cellWrapper.cell?.title ?? ""
                                        color: Theme.textMuted
                                        font: titleMetrics.font
                                        elide: Text.ElideRight
                                        maximumLineCount: 1
                                        horizontalAlignment: Text.AlignHCenter
                                    }

                                    Text {
                                        visible: rowItem.rowHasSubtitle
                                        y: rowItem.cellHeight + root._textGap + (rowItem.rowHasTitle ? titleMetrics.height + root._textGap : 0)
                                        width: rowItem.cellWidth
                                        height: subtitleMetrics.height
                                        text: cellWrapper.cell?.subtitle ?? ""
                                        color: Theme.textMuted
                                        font: subtitleMetrics.font
                                        elide: Text.ElideRight
                                        maximumLineCount: 1
                                        horizontalAlignment: Text.AlignHCenter
                                        opacity: 0.7
                                    }

                                    DraggableMouseArea {
                                        id: cellMouseArea
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        draggable: cellWrapper.cell?.draggable ?? false
                                        onItemClicked: {
                                            if (root.cmdModel) {
                                                root.cmdModel.select(cellWrapper.cellSection, cellWrapper.cellItem);
                                                if (Config.activateOnSingleClick)
                                                    root.cmdModel.activateSelected();
                                            }
                                        }
                                        onItemActivated: {
                                            if (root.cmdModel) {
                                                root.cmdModel.select(cellWrapper.cellSection, cellWrapper.cellItem);
                                                root.cmdModel.activateSelected();
                                            }
                                        }
                                        onDragRequested: {
                                            if (root.cmdModel) {
                                                root.cmdModel.select(cellWrapper.cellSection, cellWrapper.cellItem);
                                                root.cmdModel.startDrag(cellWrapper.cellSection, cellWrapper.cellItem, cellWrapper);
                                            }
                                        }
                                    }

                                    ViciToolTip {
                                        readonly property string tooltipText: cellWrapper.cell?.tooltip ?? ""
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
                const row = root.cmdModel ? root.cmdModel.flatRowForSelection() : -1;
                if (row >= 0) {
                    const scrollTarget = root.cmdModel.flatRowForSelection(viewport.topInset > 0);
                    var mode = ListView.Contain;
                    if (root.cmdModel.alignSelectionScrollToTop() && !viewport.isIndexVisible(scrollTarget)) {
                        mode = ListView.Beginning;
                    }
                    viewport.revealIndex(scrollTarget, mode, row);
                }
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
