import QtQuick
import QtQuick.Effects

MultiEffect {
    id: root
    required property real topInset
    required property real bottomInset

    readonly property real topFadeEnd: topInset > 0 ? Math.min(topInset + 32, height / 2) : 0
    readonly property real bottomFadeStart: bottomInset > 0 ? Math.max(height - bottomInset - 36, topFadeEnd) : height

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
                position: Math.min(root.topFadeEnd, Math.max(0, root.topInset - 40)) / mask.height
                color: root.topInset > 0 ? "transparent" : "white"
            }
            GradientStop {
                position: root.topFadeEnd / mask.height
                color: "white"
            }
            GradientStop {
                position: root.bottomFadeStart / mask.height
                color: "white"
            }
            GradientStop {
                position: Math.min(mask.height, Math.max(root.bottomFadeStart, root.height - root.bottomInset + 28)) / mask.height
                color: root.bottomInset > 0 ? "transparent" : "white"
            }
        }
    }
}
