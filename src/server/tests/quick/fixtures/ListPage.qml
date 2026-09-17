import QtQuick
import QtQuick.Controls
import Vicinae

ScrollViewport {
    id: root
    flickable: list
    topPadding: 10
    bottomPadding: 10
    property int initialIndex: -1

    Component.onCompleted: if (initialIndex >= 0)
        revealIndex(initialIndex, ListView.Beginning)

    ListView {
        id: list
        anchors.fill: parent
        clip: true
        model: 200
        currentIndex: 1
        reuseItems: true
        cacheBuffer: 200
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}
        delegate: Rectangle {
            required property int index
            width: ListView.view.width
            height: index % 6 === 0 ? 24 : 60
            color: index % 6 === 0 ? "green" : "red"
        }
    }
}
