import QtQuick
import Vicinae

GenericGridView {
    columns: 6
    showCellTitle: true

    cellDelegate: Component {
        Item {
            id: cellRoot
            readonly property var model: parent ? parent.cmdModel : null
            readonly property int sec: parent ? parent.cellSection : 0
            readonly property int item: parent ? parent.cellItem : 0

            ViciImage {
                anchors.fill: parent
                source: cellRoot.model ? cellRoot.model.fontIcon(cellRoot.sec, cellRoot.item) : ""
                fillMode: Image.PreserveAspectFit
                sourceSize: Qt.size(96, 96)
            }
        }
    }
}
