pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

ViciMenu {
    id: root
    required property attachmentContent content
    property bool previewable: true
    property bool removable: false
    signal previewRequested
    signal removeRequested

    Instantiator {
        model: [
            {
                text: qsTr("Open"),
                available: root.content.canOpen,
                run: () => root.content.open()
            },
            {
                text: qsTr("Preview Image"),
                available: root.previewable && root.content.image,
                run: () => root.previewRequested()
            },
            {
                text: qsTr("Copy File"),
                available: root.content.hasFile,
                run: () => root.content.copyFile()
            },
            {
                text: root.content.revealLabel,
                available: root.content.hasFile,
                run: () => root.content.reveal()
            },
            {
                text: qsTr("Copy Link"),
                available: root.content.hasLink,
                run: () => root.content.copyLink()
            },
            {
                text: root.content.image ? qsTr("Copy Image") : qsTr("Copy Text"),
                available: root.content.canCopyContents,
                run: () => root.content.copyContents()
            },
            {
                text: qsTr("Remove Attachment"),
                available: root.removable,
                run: () => root.removeRequested()
            }
        ].filter(action => action.available)
        delegate: ViciMenuItem {
            required property var modelData
            text: modelData.text
            onTriggered: modelData.run()
        }
        onObjectAdded: (index, object) => root.insertItem(index, object)
        onObjectRemoved: (index, object) => root.removeItem(object)
    }
}
