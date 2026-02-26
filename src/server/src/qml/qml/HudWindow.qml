import QtQuick
import QtQuick.Window
import QtQuick.Layouts

Window {
    id: hudRoot
    width: pill.width
    height: pill.height
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.ToolTip
    color: "transparent"
    visible: false

    Component.onCompleted: hud.registerWindow(hudRoot)

    Rectangle {
        id: pill
        width: row.width + 30
        height: row.height + 20
        radius: height / 2
        color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.9)
        border.color: Theme.divider
        border.width: 1

        RowLayout {
            id: row
            anchors.centerIn: parent
            spacing: 5

            ViciImage {
                visible: hud.hasIcon
                source: hud.icon
                Layout.preferredWidth: 16
                Layout.preferredHeight: 16
                sourceSize: Qt.size(16, 16)
            }

            Text {
                text: hud.text
                color: Theme.foreground
                font.family: Theme.fontFamily
                font.pointSize: Theme.smallerFontSize
                maximumLineCount: 1
                elide: Text.ElideRight
                Layout.maximumWidth: 270
            }
        }
    }
}
