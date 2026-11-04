pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Row {
    id: root

    property real level: 0.0
    property bool active: true
    property bool ready: true
    property int barCount: 9
    property int barWidth: 4
    property int maxHeight: 18
    property real armingPhase: 0
    property var levels: []

    spacing: 3
    height: maxHeight

    function reset() {
        levels = Array(barCount).fill(0);
    }

    onActiveChanged: reset()
    onReadyChanged: reset()
    onBarCountChanged: reset()
    Component.onCompleted: reset()

    // A short history of the volume gives each bar real motion without inventing audio activity.
    Timer {
        interval: 50
        repeat: true
        running: root.active && root.ready
        onTriggered: {
            const samples = root.levels.slice(1);
            samples.push(Math.max(0, Math.min(1, root.level)));
            root.levels = samples;
        }
    }

    NumberAnimation on armingPhase {
        running: root.active && !root.ready
        from: 0
        to: 2 * Math.PI
        duration: 900
        loops: Animation.Infinite
    }

    Repeater {
        model: root.barCount

        Rectangle {
            id: bar
            required property int index
            readonly property real sample: root.levels[index] ?? 0

            width: root.barWidth
            height: root.ready ? 3 + (root.maxHeight - 3) * Math.pow(sample, 0.7) : 5 + 2 * Math.sin(root.armingPhase - index * 0.5)
            radius: bar.width / 2
            color: root.ready ? Theme.accent : Theme.textMuted
            opacity: root.ready ? 1 : 0.5
            anchors.verticalCenter: parent.verticalCenter

            Behavior on height {
                enabled: root.ready
                NumberAnimation {
                    duration: 80
                    easing.type: Easing.OutQuad
                }
            }
            Behavior on color {
                ColorAnimation {
                    duration: 180
                }
            }
        }
    }
}
