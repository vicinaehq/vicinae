pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root

    property string imageSource
    property string textContent
    property string mimeType

    Loader {
        anchors.fill: parent
        active: root.imageSource !== ""
        visible: active
        sourceComponent: ImagePreview {
            source: root.imageSource
        }
    }

    Loader {
        anchors.fill: parent
        active: root.imageSource === "" && root.textContent !== ""
        visible: active
        sourceComponent: TextViewer {
            text: root.textContent
            monospace: true
        }
    }

    Loader {
        anchors.fill: parent
        active: root.imageSource === "" && root.textContent === ""
        visible: active
        sourceComponent: EmptyView {
            title: root.mimeType
            description: qsTr("Preview not available for this file type")
        }
    }
}
