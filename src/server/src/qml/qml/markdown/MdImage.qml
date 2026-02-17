import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root

    property var blockData: ({})
    property var mdModel: null
    property int blockIndex: -1

    readonly property int requestedWidth: blockData.width ?? 0
    readonly property int requestedHeight: blockData.height ?? 0
    readonly property string alt: blockData.alt ?? ""
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
            Layout.preferredWidth: imgW
            Layout.preferredHeight: imgH

            readonly property int imgW: {
                if (root.requestedWidth > 0) return root.requestedWidth
                if (root.requestedHeight > 0) return root.requestedHeight
                return root.width
            }
            readonly property int imgH: {
                if (root.requestedHeight > 0) return root.requestedHeight
                if (root.requestedWidth > 0) return root.requestedWidth
                return 200
            }

            Image {
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source: root.blockData.src ?? ""
                asynchronous: true
                cache: true
            }

        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            visible: root.alt.length > 0
            text: root.alt
            color: Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            font.italic: true
        }
    }
}
