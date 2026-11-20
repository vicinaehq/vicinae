pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Loader {
    id: root
    required property MarkdownModel actions
    property string imageSource: ""
    property string link: ""
    active: false

    function show(source, link) {
        root.imageSource = source;
        root.link = link;
        if (active)
            (item as ImageViewer).showImage(0, [source]);
        else
            active = true;
    }

    sourceComponent: ImageViewer {
        id: viewer
        contextMenu: MarkdownImageMenu {
            source: root.imageSource
            link: root.link
            actions: root.actions
            previewable: false
        }
        Component.onCompleted: showImage(0, [root.imageSource])
        onClosed: root.active = false
        Connections {
            target: root.Window.window
            function onVisibleChanged() {
                if (!root.Window.window?.visible)
                    viewer.close();
            }
        }
    }
}
