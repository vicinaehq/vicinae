pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Row {
    id: root
    property bool active: true
    spacing: 4

    Repeater {
        model: 3
        Rectangle {
            id: dot
            required property int index
            width: 6
            height: 6
            radius: 3
            color: Theme.foreground
            opacity: 0.3

            SequentialAnimation on opacity {
                running: root.active
                loops: Animation.Infinite
                PauseAnimation {
                    duration: dot.index * 200
                }
                NumberAnimation {
                    to: 0.8
                    duration: 400
                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    to: 0.3
                    duration: 400
                    easing.type: Easing.InOutQuad
                }
                PauseAnimation {
                    duration: (2 - dot.index) * 200
                }
            }
        }
    }
}
