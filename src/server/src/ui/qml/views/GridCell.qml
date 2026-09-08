pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Loader {
    property int cellSection
    property int cellItem
    property bool cellSelected
    property bool cellHovered
    property real cellSize
    property real cellWidth
    property real cellHeight
    property SectionGridModel cmdModel
}
