pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

GenericGridView {
    columns: 8

    cellDelegate: Component {
        Item {
            id: cellRoot
            readonly property GridCell host: parent as GridCell
            readonly property EmojiGridModel model: host ? (host.cmdModel as EmojiGridModel) : null
            readonly property int sec: host ? host.cellSection : 0
            readonly property int item: host ? host.cellItem : 0

            ViciImage {
                anchors.fill: parent
                source: cellRoot.model ? cellRoot.model.emojiIcon(cellRoot.sec, cellRoot.item) : ""
                fillMode: Image.PreserveAspectFit
                sourceSize: Qt.size(64, 64)
            }
        }
    }
}
