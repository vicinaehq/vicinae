import QtQuick
import QtQuick.Controls

Item {
    id: root

    // The backing model — must be a QAbstractListModel with roles:
    //   isSection, sectionName, rowSectionIdx, rowStartItem, rowItemCount
    // AND expose Q_INVOKABLEs for selection/navigation:
    //   selectedSection, selectedItem, select(), activateSelected(),
    //   navigateUp/Down/Left/Right(), flatRowForSelection()
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
    property real cellInset: 0.10  // ratio of cell size (None≈0.05, Small=0.10, Medium=0.15, Large=0.25)

    // Optional title/subtitle below each cell.
    // When enabled, the model must provide Q_INVOKABLE cellTitle(section, item)
    // and/or cellSubtitle(section, item).
    property bool showCellTitle: false
    property bool showCellSubtitle: false

    // Empty view — shown when the grid has no items.
    property string emptyTitle: "No results"
    property string emptyDescription: ""
    property string emptyIcon: "image://vicinae/builtin:magnifying-glass?fg=" + Theme.foreground
    property Component emptyViewComponent: null

    property bool suppressEmpty: false

    readonly property bool _empty: listView.count === 0

    readonly property real cellSize:
        Math.floor((root.width - horizontalPadding * 2 - cellSpacing * (columns - 1)) / columns)

    // Hidden TextMetrics to measure actual line heights from the font
    TextMetrics { id: _titleMetrics;    font.pointSize: Theme.smallerFontSize }
    TextMetrics { id: _subtitleMetrics; font.pointSize: Theme.smallerFontSize - 1 }

    readonly property real _textGap: 4
    readonly property real cellTextHeight: {
        if (!showCellTitle && !showCellSubtitle) return 0
        var h = _textGap
        if (showCellTitle) h += _titleMetrics.font.pixelSize
        if (showCellSubtitle) h += _subtitleMetrics.font.pixelSize
        return h
    }
    readonly property real rowHeight: cellSize + cellTextHeight

    function moveUp() { if (cmdModel) cmdModel.navigateUp() }
    function moveDown() { if (cmdModel) cmdModel.navigateDown() }
    function moveLeft() { if (cmdModel) cmdModel.navigateLeft() }
	function moveRight() { if (cmdModel) cmdModel.navigateRight() }

    ListView {
        id: listView
        anchors.fill: parent
        visible: !root._empty
        model: root.cmdModel
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        spacing: root.cellSpacing
        reuseItems: true
        cacheBuffer: 200

        ScrollBar.vertical: ScrollBar {
            policy: listView.contentHeight > listView.height
                    ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
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

            sourceComponent: isSection ? sectionComponent : rowComponent

            Component {
                id: sectionComponent
                SectionHeader {
                    width: delegateLoader.width
                    text: delegateLoader.sectionName
                }
			}

            Component {
                id: rowComponent
                Item {
                    id: rowItem
                    width: delegateLoader.width

                    readonly property int effectiveCols: delegateLoader.rowColumns
                    readonly property real effectiveAspectRatio: delegateLoader.rowAspectRatio
                    readonly property real cellWidth:
                        Math.floor((root.width - root.horizontalPadding * 2
                         - root.cellSpacing * (effectiveCols - 1)) / effectiveCols)
                    readonly property real cellHeight: Math.floor(cellWidth / effectiveAspectRatio)

                    readonly property real cellTextHeight: {
                        if (!root.showCellTitle && !root.showCellSubtitle) return 0
                        var h = root._textGap
                        if (root.showCellTitle) h += _titleMetrics.font.pixelSize
                        if (root.showCellSubtitle) h += _subtitleMetrics.font.pixelSize
                        return h
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
                                readonly property bool cellSelected:
                                    root.cmdModel
                                    && root.cmdModel.selectedSection === cellSection
                                    && root.cmdModel.selectedItem === cellItem

                                width: rowItem.cellWidth
                                height: rowItem.cellHeight + rowItem.cellTextHeight

                                // Cell background (no border — overlay handles it)
                                SourceBlendRect {
                                    id: cellBackground
                                    width: rowItem.cellWidth
                                    height: rowItem.cellHeight
                                    radius: 10
                                    backgroundColor: {
                                        var bg = Theme.background
                                        return Qt.rgba(bg.r, bg.g, bg.b, Config.windowOpacity)
                                    }
                                    color: {
                                        var bg = Theme.gridItemBackground
                                        return Qt.rgba(bg.r, bg.g, bg.b, Config.windowOpacity)
                                    }
                                }

                                // Delegate content — inside the cell with inset margins
                                Loader {
                                    x: rowItem.cellWidth * root.cellInset
                                    y: rowItem.cellHeight * root.cellInset
                                    width: rowItem.cellWidth * (1 - 2 * root.cellInset)
                                    height: rowItem.cellHeight * (1 - 2 * root.cellInset)
                                    sourceComponent: root.cellDelegate
                                    property int cellSection: cellWrapper.cellSection
                                    property int cellItem: cellWrapper.cellItem
                                    property bool cellSelected: cellWrapper.cellSelected
                                    property bool cellHovered: cellMouseArea.containsMouse
                                    property real cellSize: rowItem.cellWidth
                                    property real cellWidth: rowItem.cellWidth
                                    property real cellHeight: rowItem.cellHeight
                                    property var cmdModel: root.cmdModel
                                }

                                // Overlay — draws selection/hover border on top of content.
                                // Expanded by 2px so the stroke's outer half fits.
                                SourceBlendRect {
                                    readonly property real pad: 2
                                    x: -pad
                                    y: -pad
                                    width: rowItem.cellWidth + pad * 2
                                    height: rowItem.cellHeight + pad * 2
                                    radius: 10
                                    overlay: true
                                    borderWidth: (cellWrapper.cellSelected || cellMouseArea.containsMouse) ? 2 : 0
                                    borderColor: cellWrapper.cellSelected
                                                 ? Theme.gridItemSelectionOutline
                                                 : Theme.gridItemHoverOutline
                                }

                                // Title below the cell
                                Text {
                                    visible: root.showCellTitle
                                    y: rowItem.cellHeight + root._textGap
                                    width: rowItem.cellWidth
                                    height: _titleMetrics.font.pixelSize
                                    text: (root.cmdModel && typeof root.cmdModel.cellTitle === "function")
                                          ? root.cmdModel.cellTitle(cellWrapper.cellSection, cellWrapper.cellItem) : ""
                                    color: Theme.textMuted
                                    font: _titleMetrics.font
                                    elide: Text.ElideRight
                                    maximumLineCount: 1
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                // Subtitle below title
                                Text {
                                    visible: root.showCellSubtitle
                                    y: rowItem.cellHeight + root._textGap
                                       + (root.showCellTitle ? _titleMetrics.font.pixelSize + root._textGap : 0)
                                    width: rowItem.cellWidth
                                    height: _subtitleMetrics.font.pixelSize
                                    text: (root.cmdModel && typeof root.cmdModel.cellSubtitle === "function")
                                          ? root.cmdModel.cellSubtitle(cellWrapper.cellSection, cellWrapper.cellItem) : ""
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
                                        if (root.cmdModel)
                                            root.cmdModel.select(cellWrapper.cellSection, cellWrapper.cellItem)
                                    }
                                    onDoubleClicked: {
                                        if (root.cmdModel) {
                                            root.cmdModel.select(cellWrapper.cellSection, cellWrapper.cellItem)
                                            root.cmdModel.activateSelected()
                                        }
                                    }
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
        function onSelectionChanged() {
            var row = root.cmdModel ? root.cmdModel.flatRowForSelection() : -1
            if (row >= 0) listView.positionViewAtIndex(row, ListView.Contain)
        }
    }

    Loader {
        anchors.fill: parent
        active: root._empty && !root.suppressEmpty
        visible: active
        sourceComponent: root.emptyViewComponent
                         ? root.emptyViewComponent
                         : defaultEmptyView
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
