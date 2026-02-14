import QtQuick
import QtQuick.Controls

GenericGridView {
    columns: cmdModel ? cmdModel.columns : 8
    aspectRatio: cmdModel ? cmdModel.aspectRatio : 1.0
    cellInset: cmdModel ? cmdModel.inset : 0.10
    showCellTitle: true
    showCellSubtitle: true

    emptyTitle: cmdModel && cmdModel.emptyTitle ? cmdModel.emptyTitle : "No results"
    emptyDescription: cmdModel ? cmdModel.emptyDescription : ""
    emptyIcon: {
        var _ = Theme.foreground
        var icon = cmdModel ? cmdModel.emptyIcon : ""
        return icon !== "" ? icon : "image://vicinae/builtin:magnifying-glass?fg=" + Theme.foreground
    }

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
                // ObjectFit enum: 0=Contain, 1=Fill, 2=Stretch
                fillMode: {
                    var fit = cellRoot.model ? cellRoot.model.fit : 0
                    if (fit === 1) return Image.PreserveAspectCrop
                    if (fit === 2) return Image.Stretch
                    return Image.PreserveAspectFit
                }
                source: {
                    // Re-evaluate when theme changes (icon URLs embed tint color)
                    var _ = Theme.foreground
                    return cellRoot.model ? cellRoot.model.cellIcon(cellRoot.sec, cellRoot.itm) : ""
                }
                sourceSize.width: cellRoot.parent ? cellRoot.parent.cellWidth : width
                sourceSize.height: cellRoot.parent ? cellRoot.parent.cellHeight : height
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
