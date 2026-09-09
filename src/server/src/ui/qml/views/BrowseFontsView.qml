pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

GenericGridView {
    columns: 6
    showCellTitle: true

    cellDelegate: Component {
        Item {
            id: cellRoot
            readonly property GridCell host: parent as GridCell
            readonly property FontGridModel model: host ? (host.cmdModel as FontGridModel) : null
            readonly property int sec: host ? host.cellSection : 0
            readonly property int item: host ? host.cellItem : 0

            ViciImage {
                anchors.fill: parent
                source: cellRoot.model ? cellRoot.model.fontIcon(cellRoot.sec, cellRoot.item) : ""
                fillMode: Image.PreserveAspectFit
                sourceSize: Qt.size(96, 96)
            }
        }
    }
}
