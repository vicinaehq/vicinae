pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Loader {
    id: root
    property var _content: null
    active: false

    function show(content) {
        _content = content;
        if (active)
            (item as ImageViewer).showImage(0, [content.imageSource()]);
        else
            active = true;
    }

    sourceComponent: ImageViewer {
        id: viewer
        contextMenu: AttachmentMenu {
            content: root._content
            previewable: false
        }
        Component.onCompleted: showImage(0, [root._content.imageSource()])
        onClosed: {
            root.active = false;
            root._content = null;
        }
        Connections {
            target: root.Window.window
            function onVisibleChanged() {
                if (!root.Window.window?.visible)
                    viewer.close();
            }
        }
    }
}
