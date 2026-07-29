import QtQuick
import QtQuick.Window
import QtQuick.Layouts

Window {
    id: root

    property color pillColor: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.9)
    property color pillBorderColor: Config.withAlpha(Theme.divider, Config.windowOpacity)
    property int pillBorderWidth: 1
    property real pillRadius: pill.height / 2

    signal shown

    width: pill.width
    height: pill.height
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.ToolTip
    color: "transparent"
    visible: false

    onVisibleChanged: if (visible)
        shown()

    Component.onCompleted: hud.registerWindow(root)

    Rectangle {
        id: pill
        width: row.width + 30
        height: row.height + 20
        radius: root.pillRadius
        color: root.pillColor
        border.color: root.pillBorderColor
        border.width: root.pillBorderWidth

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
