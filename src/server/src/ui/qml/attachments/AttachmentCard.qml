pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

Rectangle {
    id: root
    required property var attachment
    property bool removable: false
    signal removeRequested(int attachmentId)
    signal previewRequested(var content)
    activeFocusOnTab: root.attachment.content.image
    Accessible.role: root.attachment.content.image ? Accessible.Button : Accessible.StaticText
    Accessible.name: root.attachment.name
    Accessible.description: root.attachment.error || root.attachment.detail || ""
    Accessible.onPressAction: root.openPreview()
    Keys.onReturnPressed: root.openPreview()
    Keys.onSpacePressed: root.openPreview()

    function openPreview() {
        if (root.attachment.content.image)
            root.previewRequested(root.attachment.content);
    }

    ContextMenu.menu: AttachmentMenu {
        content: root.attachment.content
        removable: root.removable
        onPreviewRequested: root.openPreview()
        onRemoveRequested: root.removeRequested(root.attachment.id)
    }

    implicitWidth: 216
    implicitHeight: 64
    radius: 8
    color: Config.withAlpha(Theme.foreground, root.activeFocus ? 0.1 : 0.05)
    border.width: 1
    border.color: root.attachment.error ? Theme.danger : root.activeFocus ? Theme.buttonFocusOutline : Theme.divider

    MouseArea {
        anchors.fill: parent
        enabled: !root.DocumentScope.measuring
        acceptedButtons: Qt.LeftButton
        cursorShape: root.attachment.content.image ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: root.openPreview()
    }

    ViciImage {
        x: 8
        y: 8
        width: 48
        height: 48
        source: root.DocumentScope.measuring || root.attachment.loading ? "" : root.attachment.preview
        fillMode: ViciImage.PreserveAspectFit
        visible: !root.attachment.loading
    }
    ViciSpinner {
        x: 22
        y: 22
        width: 20
        height: 20
        visible: root.attachment.loading ?? false
    }
    Column {
        x: 64
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - x - (root.removable ? 32 : 8)
        spacing: 4
        Text {
            width: parent.width
            text: root.attachment.name ?? ""
            textFormat: Text.PlainText
            font.family: Theme.fontFamily
            font.pointSize: Theme.regularFontSize
            color: Theme.foreground
            elide: Text.ElideMiddle
        }
        Text {
            width: parent.width
            text: root.attachment.error || (root.attachment.loading ? qsTr("Preparing…") : root.attachment.detail ?? "")
            textFormat: Text.PlainText
            font.family: Theme.fontFamily
            font.pointSize: Theme.smallerFontSize
            color: root.attachment.error ? Theme.danger : Theme.textMuted
            elide: Text.ElideRight
        }
    }
    ViciButton {
        visible: root.removable
        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.verticalCenter: parent.verticalCenter
        implicitWidth: 22
        implicitHeight: 22
        iconSize: 12
        iconSource: Img.icon(BuiltinIcon.Xmark)
        accessibleName: qsTr("Remove %1").arg(root.attachment.name ?? "")
        activeFocusOnTab: true
        onClicked: root.removeRequested(root.attachment.id)
    }
    HoverHandler {
        id: hover
    }
    ToolTip.visible: hover.hovered
    ToolTip.text: root.attachment.error || root.attachment.name || ""
    ToolTip.delay: 600
}
