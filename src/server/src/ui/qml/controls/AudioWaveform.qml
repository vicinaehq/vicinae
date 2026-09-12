pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Row {
    id: root

    property real level: 0.0
    property int barCount: 14
    property int barWidth: 3
    property int maxHeight: 18

    spacing: 2
    height: maxHeight

    Repeater {
        model: root.barCount

        Rectangle {
            id: bar
            required property int index

            readonly property real weight: 0.35 + 0.65 * Math.sin(Math.PI * (bar.index + 0.5) / root.barCount)

            width: root.barWidth
            height: Math.max(root.barWidth, root.maxHeight * Math.min(1, root.level * 1.6) * bar.weight)
            radius: bar.width / 2
            color: Theme.accent
            anchors.verticalCenter: parent.verticalCenter

            Behavior on height {
                NumberAnimation {
                    duration: 70
                    easing.type: Easing.OutQuad
                }
            }
        }
    }
}
