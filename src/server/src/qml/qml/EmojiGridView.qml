import QtQuick

GenericGridView {
    columns: 8
    cellInset: 0.25

    cellDelegate: Component {
        Item {
            id: cellRoot
            readonly property var model: parent ? parent.cmdModel : null
            readonly property int sec: parent ? parent.cellSection : 0
            readonly property int item: parent ? parent.cellItem : 0

            ViciImage {
                anchors.fill: parent
                source: cellRoot.model ? cellRoot.model.emojiIcon(cellRoot.sec, cellRoot.item) : ""
                fillMode: Image.PreserveAspectFit
                sourceSize: Qt.size(64, 64)
            }
        }
    }
}
