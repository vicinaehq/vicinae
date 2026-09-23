pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Flow {
    id: root
    property var attachments: []
    property bool removable: false
    signal removeRequested(int attachmentId)
    signal previewRequested(var content)
    spacing: 8
    Repeater {
        model: root.attachments
        AttachmentCard {
            required property var modelData
            width: Math.min(implicitWidth, root.width)
            attachment: modelData
            removable: root.removable
            onRemoveRequested: attachmentId => root.removeRequested(attachmentId)
            onPreviewRequested: content => root.previewRequested(content)
        }
    }
}
