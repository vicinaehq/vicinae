import QtQuick

Rectangle {
    id: root
    property real level: 0.0

    radius: height / 2
    color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.08)

    Rectangle {
        width: Math.max(parent.height, parent.width * root.level)
        height: parent.height
        radius: parent.radius
        color: Theme.accent

        Behavior on width {
            NumberAnimation {
                duration: 60
                easing.type: Easing.OutQuad
            }
        }
    }
}
