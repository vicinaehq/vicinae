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

            ViciImage {
                anchors.fill: parent
                source: cellRoot.host?.cell?.icon ?? ""
                fillMode: Image.PreserveAspectFit
                sourceSize: Qt.size(96, 96)
            }
        }
    }
}
