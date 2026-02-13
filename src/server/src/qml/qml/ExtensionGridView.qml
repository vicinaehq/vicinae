import QtQuick
import QtQuick.Controls

GenericGridView {
    columns: cmdModel ? cmdModel.columns : 8
    showCellTitle: true
    showCellSubtitle: true

    cellDelegate: Component {
        Item {
            id: cellRoot
            readonly property var model: parent ? parent.cmdModel : null
            readonly property int sec: parent ? parent.cellSection : 0
            readonly property int itm: parent ? parent.cellItem : 0
            readonly property bool hovered: parent ? parent.cellHovered : false

            Image {
                anchors.centerIn: parent
                width: parent.width
                height: parent.height
                source: {
                    // Re-evaluate when theme changes (icon URLs embed tint color)
                    var _ = Theme.foreground
                    return cellRoot.model ? cellRoot.model.cellIcon(cellRoot.sec, cellRoot.itm) : ""
                }
                sourceSize.width: width
                sourceSize.height: height
                asynchronous: true
                cache: true
            }

            ToolTip {
                visible: cellRoot.hovered && tooltipText !== ""
                readonly property string tooltipText: cellRoot.model ? cellRoot.model.cellTooltip(cellRoot.sec, cellRoot.itm) : ""
                text: tooltipText
                delay: 500
            }
        }
    }
}
