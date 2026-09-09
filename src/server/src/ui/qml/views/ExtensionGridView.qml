pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

GenericGridView {
    columns: cmdModel ? cmdModel.columns : 8
    aspectRatio: cmdModel ? cmdModel.aspectRatio : 1.0
    showCellTitle: true
    showCellSubtitle: true

    emptyTitle: cmdModel && cmdModel.emptyTitle ? cmdModel.emptyTitle : qsTr("No results")
    emptyDescription: cmdModel ? cmdModel.emptyDescription : ""
    emptyIcon: cmdModel?.emptyIcon.valid ? cmdModel.emptyIcon : Img.icon(BuiltinIcon.MagnifyingGlass).withFillColor(Theme.foreground)

    cellDelegate: Component {
        Item {
            id: cellRoot
            readonly property GridCell host: parent as GridCell
            readonly property ExtensionGridModel model: host ? (host.cmdModel as ExtensionGridModel) : null
            readonly property int sec: host ? host.cellSection : 0
            readonly property int itm: host ? host.cellItem : 0
            readonly property string _cellColor: {
                var _rev = cellRoot.model ? cellRoot.model.dataRevision : 0;
                return cellRoot.model ? cellRoot.model.cellColor(cellRoot.sec, cellRoot.itm) : "";
            }

            readonly property string _imageSource: {
                var _ = Theme.foreground;
                var _rev = cellRoot.model ? cellRoot.model.dataRevision : 0;
                return cellRoot.model ? cellRoot.model.cellIcon(cellRoot.sec, cellRoot.itm) : "";
            }

            readonly property size _sourceSize: Qt.size(cellRoot.host ? cellRoot.host.cellWidth : width, cellRoot.host ? cellRoot.host.cellHeight : height)

            Rectangle {
                visible: cellRoot._cellColor !== ""
                anchors.fill: parent
                radius: 6
                color: cellRoot._cellColor
            }

            ViciImage {
                visible: cellRoot._cellColor === ""
                anchors.fill: parent
                // ObjectFit enum: 0=Contain, 1=Fill, 2=Stretch
                fillMode: {
                    var fit = cellRoot.model ? cellRoot.model.fit : 0;
                    if (fit === 1)
                        return Image.PreserveAspectCrop;
                    if (fit === 2)
                        return Image.Stretch;
                    return Image.PreserveAspectFit;
                }
                source: cellRoot._imageSource
                sourceSize: cellRoot._sourceSize
            }
        }
    }
}
