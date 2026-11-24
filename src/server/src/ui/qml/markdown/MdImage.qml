pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

Item {
    id: root

    property var blockData: ({})
    property MarkdownModel actions: null
    property real maxImageHeight: 200
    property real fontSize: Theme.smallerFontSize
    signal previewRequested(string source, string link)

    readonly property real requestedWidth: blockData.width ?? 0
    readonly property real requestedHeight: blockData.height ?? 0
    readonly property string source: blockData.src ?? ""
    readonly property string alt: blockData.alt ?? ""
    readonly property string link: blockData.link ?? ""
    readonly property bool ready: image.status === ViciImage.Ready
    readonly property real naturalWidth: image.implicitWidth > 0 ? image.implicitWidth : Math.max(1, requestedWidth || width)
    readonly property real naturalHeight: image.status === ViciImage.Error ? 72 : image.implicitHeight > 0 ? image.implicitHeight : Math.max(1, requestedHeight || 200)
    readonly property real aspectRatio: naturalWidth / naturalHeight
    readonly property real preferredWidth: requestedWidth > 0 ? requestedWidth : requestedHeight > 0 ? requestedHeight * aspectRatio : naturalWidth

    width: parent?.width ?? 0
    implicitHeight: imageFrame.height + (caption.visible ? caption.implicitHeight + 8 : 0)

    function openPreview() {
        if (ready)
            previewRequested(source, link);
    }

    Item {
        id: imageFrame
        anchors.horizontalCenter: parent.horizontalCenter
        width: Math.max(0, Math.min(root.width, root.preferredWidth, root.requestedHeight > 0 ? root.requestedHeight * root.aspectRatio : Infinity, root.maxImageHeight > 0 ? root.maxImageHeight * root.aspectRatio : Infinity))
        height: width / root.aspectRatio
        activeFocusOnTab: root.ready && !root.DocumentScope.measuring
        Accessible.role: root.ready ? Accessible.Button : Accessible.StaticText
        Accessible.name: root.alt || qsTr("Image")
        Accessible.description: root.ready ? qsTr("Preview image") : ""
        Accessible.ignored: root.DocumentScope.measuring
        Accessible.onPressAction: root.openPreview()
        Keys.onReturnPressed: root.openPreview()
        Keys.onSpacePressed: root.openPreview()

        Rectangle {
            anchors.fill: parent
            radius: 8
            color: Config.withAlpha(Theme.foreground, 0.04)
            visible: !root.ready
        }

        ViciImage {
            id: image
            anchors.fill: parent
            fillMode: ViciImage.PreserveAspectFit
            fallbackEnabled: false
            sourceSize: Qt.size(Math.max(1, root.width), Math.max(1, root.maxImageHeight > 0 ? root.maxImageHeight : root.width))
            source: root.DocumentScope.measuring ? "" : root.source
        }

        ViciSpinner {
            anchors.centerIn: parent
            width: 20
            height: 20
            visible: image.status === ViciImage.Loading
        }

        Text {
            anchors.centerIn: parent
            width: Math.max(0, parent.width - 24)
            visible: image.status === ViciImage.Error
            text: qsTr("Image unavailable")
            textFormat: Text.PlainText
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pointSize: root.fontSize
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
        }

        Rectangle {
            anchors.fill: parent
            color: Theme.textSelectionBg
            opacity: 0.4
            visible: selection.hasSelection
        }

        Rectangle {
            anchors.fill: parent
            anchors.margins: -3
            radius: 4
            color: "transparent"
            border.width: 1
            border.color: Theme.buttonFocusOutline
            visible: imageFrame.activeFocus
        }

        HoverHandler {
            id: hover
            enabled: root.ready
            cursorShape: Qt.PointingHandCursor
        }

        DraggableMouseArea {
            anchors.fill: parent
            enabled: root.ready && !root.DocumentScope.measuring
            acceptedButtons: Qt.LeftButton
            draggable: root.actions !== null
            onItemClicked: root.openPreview()
            onDragRequested: root.actions?.dragImage(root.source, imageFrame)
        }

        Rectangle {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 8
            width: 28
            height: 28
            radius: 14
            color: Config.withAlpha(Theme.background, 0.9)
            visible: root.ready && (hover.hovered || imageFrame.activeFocus) && imageFrame.width >= 64 && imageFrame.height >= 64

            ViciImage {
                anchors.centerIn: parent
                width: 14
                height: 14
                source: Img.icon(BuiltinIcon.ArrowsExpand).withFillColor(Theme.foreground)
            }
        }

        ToolTip.visible: hover.hovered && !imageMenu.visible
        ToolTip.text: root.actions ? qsTr("Click to preview. Drag to copy.") : qsTr("Preview image")
        ToolTip.delay: 600

        ContextMenu.menu: MarkdownImageMenu {
            id: imageMenu
            onAboutToShow: imageFrame.forceActiveFocus()
            source: root.source
            link: root.link
            actions: root.actions
            previewable: root.ready
            onPreviewRequested: root.openPreview()
        }
    }

    Text {
        id: caption
        anchors.top: imageFrame.bottom
        anchors.topMargin: 8
        anchors.horizontalCenter: parent.horizontalCenter
        width: root.width
        visible: root.alt.length > 0
        text: root.alt
        textFormat: Text.PlainText
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pointSize: root.fontSize
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
    }

    DocumentSelection {
        id: selection
        selectedText: root.alt || qsTr("Image")
    }
}
