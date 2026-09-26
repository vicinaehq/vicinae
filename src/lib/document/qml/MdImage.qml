pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae.Documents

Item {
    id: root
    readonly property DocumentStyle style: root.DocumentScope.style

    property Component imageDelegate: null
    property var blockData: ({})
    readonly property bool selected: selection.hasSelection
    property real maxImageHeight: 0
    property real fontSize: root.style.smallerFontSize

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
                if (root.requestedWidth > 0)
                    return root.requestedWidth;
                if (root.requestedHeight > 0)
                    return root.requestedHeight;
                return root.width;
            }
            readonly property int _rawH: {
                if (root.requestedHeight > 0)
                    return root.requestedHeight;
                if (root.requestedWidth > 0)
                    return root.requestedWidth;
                return 200;
            }
            readonly property int imgH: root.maxImageHeight > 0 ? Math.min(_rawH, 200, root.maxImageHeight) : Math.min(_rawH, 200)

            Loader {
                id: imageLoader
                anchors.fill: parent
                sourceComponent: root.imageDelegate ?? defaultImage
            }
            Binding {
                target: imageLoader.item
                property: "imageUrl"
                value: root.DocumentScope.measuring ? "" : root.blockData.src ?? ""
                when: imageLoader.item !== null
            }

            Rectangle {
                anchors.fill: parent
                color: root.style.textSelectionBg
                opacity: 0.4
                visible: root.selected
            }
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            visible: root.alt.length > 0
            text: root.alt
            color: root.style.textMuted
            font.pointSize: root.fontSize
            font.italic: true
        }
    }

    Component {
        id: defaultImage
        Image {
            property url imageUrl
            source: imageUrl
            asynchronous: true
            fillMode: Image.PreserveAspectFit
        }
    }

    DocumentSelection {
        id: selection
        selectedText: root.alt || qsTr("Image")
    }
}
