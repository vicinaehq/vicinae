import QtQuick
import Vicinae

Item {
    id: root

    property bool filled: false
    property bool hasError: false
    property bool focused: false
    property bool dimmed: false
    property real radius: 8
    property alias opaque: background.opaque

    FormInputBackground {
        id: background
        anchors.fill: parent
        radius: root.radius
        filled: root.filled
        opacity: root.dimmed ? 0.5 : 1.0
    }

    Rectangle {
        anchors.fill: parent
        radius: root.radius
        color: "transparent"
        border.color: Config.withAlpha(root.hasError ? Theme.inputBorderError : root.focused ? Theme.inputBorderFocus : Theme.inputBorder, Config.surfaceOpacity)
        border.width: 1
        opacity: root.dimmed ? 0.5 : 1.0
    }
}
