pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    property alias source: image.source
    property alias cache: image.cache

    ViciImage {
        id: image
        anchors.fill: parent
        anchors.margins: 10
        fillMode: Image.PreserveAspectFit
        sourceSize: Qt.size(width, height)
    }
}
