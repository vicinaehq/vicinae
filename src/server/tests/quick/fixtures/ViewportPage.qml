import QtQuick
import QtQuick.Controls
import Vicinae

Item {
    id: root
    property real toolbarHeight: 0
    property int activations: 0
    property alias viewport: viewport
    property alias nested: nested
    property alias preview: preview
    property alias rightViewport: rightViewport

    function restoreFocus() {
        activations++;
    }

    Rectangle {
        id: preview
        x: 250
        width: 200
        height: 150
        color: "blue"
    }

    ScrollViewport {
        id: viewport
        y: root.toolbarHeight
        width: 240
        height: root.height - y
        flickable: flick
        topPadding: 4
        bottomPadding: 4

        Flickable {
            id: flick
            anchors.fill: parent
            clip: true
            contentHeight: 1200
            boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: ScrollBar {}

            Rectangle {
                width: flick.width - 10
                height: flick.contentHeight
                color: "red"
            }

            ScrollViewport {
                id: nested
                y: 200
                width: 100
                height: 100
                flickable: nestedFlick

                Flickable {
                    id: nestedFlick
                    anchors.fill: parent
                    contentHeight: 300
                }
            }
        }
    }

    ScrollViewport {
        id: rightViewport
        x: 250
        width: 200
        height: root.height
        flickable: rightFlick

        Flickable {
            id: rightFlick
            anchors.fill: parent
            contentHeight: 900
            clip: true
        }
    }
}
