pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

Menu {
    id: root
    required property attachmentContent content
    property bool previewable: true
    property bool removable: false
    signal previewRequested
    signal removeRequested
    popupType: Popup.Native

    Instantiator {
        model: [
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
                text: root.content.image ? qsTr("Copy Image") : qsTr("Copy Text"),
                available: root.content.valid,
                run: () => root.content.copyContents()
            },
            {
                text: qsTr("Remove Attachment"),
                available: root.removable,
                run: () => root.removeRequested()
            }
        ].filter(action => action.available)
        delegate: MenuItem {
            required property var modelData
            text: modelData.text
            onTriggered: modelData.run()
        }
        onObjectAdded: (index, object) => root.insertItem(index, object)
        onObjectRemoved: (index, object) => root.removeItem(object)
    }
}
