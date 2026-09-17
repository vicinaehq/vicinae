pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    property alias source: image.source
    property alias cache: image.cache

    SearchBarInset {
        id: searchBarInset
    }

    ViciImage {
        id: image
        anchors.fill: parent
        anchors.margins: 10
        anchors.topMargin: 10 + searchBarInset.value
        fillMode: Image.PreserveAspectFit
        sourceSize: Qt.size(width, height)
    }
}
