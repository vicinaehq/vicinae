pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Effects

MultiEffect {
    id: root

    property real topFade: 0
    property real bottomFade: 0

    autoPaddingEnabled: false
    maskEnabled: true
    maskThresholdMin: 0.5
    maskSpreadAtMin: 1
    maskSource: mask

    Rectangle {
        id: mask
        width: 1
        height: Math.max(1, root.height)
        visible: false
        layer.enabled: true
        gradient: Gradient {
            GradientStop {
                position: 0
                color: root.topFade > 0 ? "transparent" : "white"
            }
            GradientStop {
                position: Math.min(root.topFade / mask.height, 0.5)
                color: "white"
            }
            GradientStop {
                position: Math.max(1 - root.bottomFade / mask.height, 0.5)
                color: "white"
            }
            GradientStop {
                position: 1
                color: root.bottomFade > 0 ? "transparent" : "white"
            }
        }
    }
}
