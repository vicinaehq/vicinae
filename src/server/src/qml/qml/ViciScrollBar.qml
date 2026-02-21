import QtQuick
import QtQuick.Controls

ScrollBar {
    id: control

    contentItem: Rectangle {
        implicitWidth: 6
        implicitHeight: 6
        radius: 3
        color: Theme.scrollBarBackground
        opacity: control.active ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
    }

    background: Item {}
}
