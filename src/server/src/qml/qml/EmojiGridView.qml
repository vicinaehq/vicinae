import QtQuick
import QtQuick.Controls

GenericGridView {
    columns: 8
    cellInset: 0.25

    cellDelegate: Component {
        Item {
            id: cellRoot
            readonly property var model: parent ? parent.cmdModel : null
            readonly property int sec: parent ? parent.cellSection : 0
            readonly property int item: parent ? parent.cellItem : 0
            readonly property bool hovered: parent ? parent.cellHovered : false

            Image {
                anchors.centerIn: parent
                width: parent.width
                height: parent.height
                source: cellRoot.model ? cellRoot.model.emojiIcon(cellRoot.sec, cellRoot.item) : ""
                sourceSize.width: width
                sourceSize.height: height
                asynchronous: true
                cache: true
            }

            ToolTip {
                visible: cellRoot.hovered
                text: cellRoot.model ? cellRoot.model.emojiName(cellRoot.sec, cellRoot.item) : ""
                delay: 500
            }
        }
    }
}
