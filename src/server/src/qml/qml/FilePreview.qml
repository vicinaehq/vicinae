import QtQuick

Item {
    id: root

    property string imageSource
    property string textContent
    property string mimeType

    Loader {
        anchors.fill: parent
        active: root.imageSource !== ""
        visible: active
        sourceComponent: Item {
            ViciImage {
                anchors.fill: parent
                anchors.margins: 10
                source: root.imageSource
                fillMode: Image.PreserveAspectFit
                sourceSize.width: width
                sourceSize.height: height
            }
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
            description: "Preview not available for this file type"
        }
    }
}
