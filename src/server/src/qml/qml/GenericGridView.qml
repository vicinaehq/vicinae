import QtQuick
import Vicinae 1.0

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
    //   cellHovered (bool), cellSize (real), cmdModel (var)
    property Component cellDelegate: null

    property int columns: 8
    property real cellSpacing: 10
    property real horizontalPadding: 20
    property real cellInset: 0.10  // ratio of cell size (None≈0.05, Small=0.10, Medium=0.15, Large=0.25)

    readonly property real cellSize:
        (root.width - horizontalPadding * 2 - cellSpacing * (columns - 1)) / columns

    function moveUp() { if (cmdModel) cmdModel.navigateUp() }
    function moveDown() { if (cmdModel) cmdModel.navigateDown() }
    function moveLeft() { if (cmdModel) cmdModel.navigateLeft() }
    function moveRight() { if (cmdModel) cmdModel.navigateRight() }

    ListView {
        id: listView
        anchors.fill: parent
        model: root.cmdModel
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        spacing: root.cellSpacing
        reuseItems: true
        cacheBuffer: 200

        delegate: Loader {
            id: delegateLoader
            width: ListView.view.width

            required property int index
            required property bool isSection
            required property string sectionName
            required property int rowSectionIdx
            required property int rowStartItem
            required property int rowItemCount

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
                    width: delegateLoader.width
                    height: root.cellSize

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

                                width: root.cellSize
                                height: root.cellSize

                                SourceBlendRect {
                                    anchors.fill: parent
                                    radius: 10
                                    backgroundColor: {
                                        var bg = Theme.background
                                        return Qt.rgba(bg.r, bg.g, bg.b, Config.windowOpacity)
                                    }
                                    color: {
                                        var bg = Theme.gridItemBackground
                                        return Qt.rgba(bg.r, bg.g, bg.b, Config.windowOpacity)
                                    }
                                    borderWidth: (cellWrapper.cellSelected || cellMouseArea.containsMouse) ? 3 : 0
                                    borderColor: cellWrapper.cellSelected
                                                 ? Theme.gridItemSelectionOutline
                                                 : Theme.gridItemHoverOutline
                                }

                                Loader {
                                    anchors.fill: parent
                                    anchors.margins: root.cellSize * root.cellInset
                                    sourceComponent: root.cellDelegate
                                    property int cellSection: cellWrapper.cellSection
                                    property int cellItem: cellWrapper.cellItem
                                    property bool cellSelected: cellWrapper.cellSelected
                                    property bool cellHovered: cellMouseArea.containsMouse
                                    property real cellSize: root.cellSize
                                    property var cmdModel: root.cmdModel
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
}
