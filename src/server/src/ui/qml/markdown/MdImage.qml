pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root

    property var blockData: ({})
    property var mdModel: null
    property int blockIndex: -1
    property var selectionController: null
    property bool selected: false
    property real maxImageHeight: 0

    readonly property int requestedWidth: blockData.width ?? 0
    readonly property int requestedHeight: blockData.height ?? 0
    readonly property int naturalWidth: blockData.naturalWidth ?? 0
    readonly property int naturalHeight: blockData.naturalHeight ?? 0
    readonly property real aspectRatio: naturalWidth > 0 && naturalHeight > 0 ? naturalWidth / naturalHeight : 0
    readonly property string link: blockData.link ?? ""

    width: parent?.width ?? 0
    implicitHeight: col.implicitHeight

    ColumnLayout {
        id: col
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 4

        Item {
            id: imgContainer
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: displaySize.width
            Layout.preferredHeight: displaySize.height

            readonly property size displaySize: root.aspectRatio > 0 ? aspectSize : fallbackSize

            readonly property size aspectSize: {
                const aspect = root.aspectRatio;
                let w = root.naturalWidth;
                if (root.requestedWidth > 0)
                    w = root.requestedWidth;
                else if (root.requestedHeight > 0)
                    w = root.requestedHeight * aspect;
                const h = root.requestedWidth > 0 && root.requestedHeight > 0 ? root.requestedHeight : w / aspect;
                const maxH = root.maxImageHeight > 0 ? root.maxImageHeight : h;
                const scale = Math.min(1, root.width / w, maxH / h);
                return Qt.size(w * scale, h * scale);
            }

            // Without known aspect ratio, reserve a bounded box
            readonly property size fallbackSize: {
                let w = root.width;
                if (root.requestedWidth > 0)
                    w = root.requestedWidth;
                else if (root.requestedHeight > 0)
                    w = root.requestedHeight;
                let rawH = 200;
                if (root.requestedHeight > 0)
                    rawH = root.requestedHeight;
                else if (root.requestedWidth > 0)
                    rawH = root.requestedWidth;
                const h = root.maxImageHeight > 0 ? Math.min(rawH, 200, root.maxImageHeight) : Math.min(rawH, 200);
                return Qt.size(w, h);
            }

            ViciImage {
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                retainFrameWhileLoading: true
                source: root.blockData.src ?? ""
            }

            Rectangle {
                anchors.fill: parent
                color: Theme.textSelectionBg
                opacity: 0.4
                visible: root.selected
            }
        }
    }

    onSelectionControllerChanged: if (selectionController)
        selectionController.registerSelectable(root, blockIndex * 10000, false)
    Component.onDestruction: if (selectionController)
        selectionController.unregisterSelectable(root)
}
