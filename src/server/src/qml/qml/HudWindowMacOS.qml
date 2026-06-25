import QtQuick
import QtQuick.Window
import QtQuick.Layouts

Window {
    id: hudRoot

    readonly property int bottomMargin: 96

    width: pill.width
    height: pill.height
    flags: Qt.Tool | Qt.FramelessWindowHint
    color: "transparent"
    visible: false

    MacOSWindow.enabled: true
    MacOSWindow.blurEnabled: true
    MacOSWindow.material: "liquidGlass"
    MacOSWindow.cornerRadius: height / 2
    MacOSWindow.borderColor: Theme.mainWindowBorder
    MacOSWindow.borderWidth: Config.borderWidth

    MacOSPanel.enabled: true
    MacOSPanel.windowLevel: MacOSPanel.Status

    Component.onCompleted: hud.registerWindow(hudRoot)

    function reposition() {
        if (visible)
            Qt.callLater(() => MacOSPanel.placeBottomCenter(bottomMargin));
    }

    onVisibleChanged: reposition()
    onWidthChanged: reposition()

    Item {
        id: pill
        width: row.width + 30
        height: row.height + 20

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
