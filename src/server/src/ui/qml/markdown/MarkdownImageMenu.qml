pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

ViciMenu {
    id: root
    required property string source
    required property string link
    required property MarkdownModel actions
    property bool previewable: true
    signal previewRequested

    Instantiator {
        model: [
            {
                text: qsTr("Preview Image"),
                available: root.previewable,
                run: () => root.previewRequested()
            },
            {
                text: qsTr("Copy Image"),
                available: root.actions !== null && root.source.length > 0,
                run: () => root.actions.copyImage(root.source)
            },
            {
                text: qsTr("Open Link"),
                available: root.actions !== null && root.link.length > 0,
                run: () => root.actions.openLink(root.link)
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
