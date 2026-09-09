import QtQuick
import Vicinae

Item {
    id: root

    property bool filled: false
    property bool hasError: false
    property bool focused: false
    property bool dimmed: false

    FormInputBackground {
        anchors.fill: parent
        radius: 8
        filled: root.filled
        opacity: root.dimmed ? 0.5 : 1.0
    }

    Rectangle {
        anchors.fill: parent
        radius: 8
        color: "transparent"
        border.color: Config.withAlpha(root.hasError ? Theme.inputBorderError : root.focused ? Theme.inputBorderFocus : Theme.inputBorder, Config.surfaceOpacity)
        border.width: 1
        opacity: root.dimmed ? 0.5 : 1.0
    }
}
