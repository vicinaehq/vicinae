pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

GenericGridView {
    columns: 3
    aspectRatio: 1.6
    showCellTitle: true
    showCellSubtitle: true
    emptyTitle: qsTr("No screenshots or recordings found")
    emptyDescription: qsTr("Save a screenshot or screen recording to a file, or try another search.")
    emptyIcon: Img.icon(BuiltinIcon.Screenshot)

    cellDelegate: Component {
        Item {
            id: cellRoot
            readonly property GridCell host: parent as GridCell

            ViciImage {
                anchors.fill: parent
                source: cellRoot.host?.cell?.icon ?? ""
                fillMode: Image.PreserveAspectFit
                sourceSize: Qt.size(512, 320)
            }
        }
    }
}
