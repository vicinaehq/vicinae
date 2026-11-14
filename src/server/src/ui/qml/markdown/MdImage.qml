pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root

    property var blockData: ({})
    readonly property bool selected: selection.hasSelection
    property real maxImageHeight: 0

    readonly property int requestedWidth: blockData.width ?? 0
    readonly property int requestedHeight: blockData.height ?? 0
    readonly property bool hasSizeHint: requestedWidth > 0 || requestedHeight > 0
    readonly property int naturalWidth: blockData.naturalWidth ?? 0
    readonly property int naturalHeight: blockData.naturalHeight ?? 0
    readonly property string alt: blockData.alt ?? ""
    readonly property string link: blockData.link ?? ""

    width: parent?.width ?? 0
    implicitHeight: imgContainer.height

    Item {
        id: imgContainer
        anchors.horizontalCenter: parent.horizontalCenter
        width: imgW
        height: imgH

        readonly property real aspect: {
            if (root.naturalWidth > 0 && root.naturalHeight > 0)
                return root.naturalWidth / root.naturalHeight;
            if (image.implicitWidth > 0 && image.implicitHeight > 0)
                return image.implicitWidth / image.implicitHeight;
            return 0;
        }

        readonly property size hintedSize: {
            let w = root.requestedWidth;
            let h = root.requestedHeight;
            if (w <= 0)
                w = aspect > 0 ? h * aspect : h;
            if (h <= 0)
                h = aspect > 0 ? w / aspect : Math.min(w, 200);
            const scale = Math.min(1, root.width / w);
            return Qt.size(Math.round(w * scale), Math.round(h * scale));
        }

        readonly property int imgW: root.hasSizeHint ? hintedSize.width : root.width
        readonly property int imgH: {
            if (root.hasSizeHint)
                return hintedSize.height;
            return root.maxImageHeight > 0 ? Math.min(200, root.maxImageHeight) : 200;
        }

        ViciImage {
            id: image
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            retainWhileLoading: true
            sourceSize: root.hasSizeHint ? Qt.size(imgContainer.imgW, imgContainer.imgH) : Qt.size(0, 0)
            source: root.DocumentScope.measuring ? "" : root.blockData.src ?? ""
        }

        Rectangle {
            anchors.fill: parent
            color: Theme.textSelectionBg
            opacity: 0.4
            visible: root.selected
        }
    }

    DocumentSelection {
        id: selection
        selectedText: root.alt || qsTr("Image")
    }
}
