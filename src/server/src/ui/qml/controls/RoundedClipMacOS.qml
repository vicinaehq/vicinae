import QtQuick
import QtQuick.Effects

MultiEffect {
    id: root
    property real cornerRadius: 10

    autoPaddingEnabled: false
    maskEnabled: true
    maskThresholdMin: 0.5
    maskSpreadAtMin: 1
    maskSource: mask

    Rectangle {
        id: mask
        width: root.width
        height: root.height
        radius: root.cornerRadius
        color: "white"
        antialiasing: true
        visible: false
        layer.enabled: true
    }
}
