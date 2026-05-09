import QtQuick
import QtQuick.Controls

ToolTip {
    id: root
    delay: 500
    popupType: Popup.Window

    contentItem: Text {
        text: root.text
        color: Theme.foreground
        font.pointSize: Theme.smallerFontSize
    }

    background: Rectangle {
        color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g, Theme.secondaryBackground.b, 0.95)
        border.color: Theme.divider
        border.width: 1
        radius: 4
        BackgroundEffect.enabled: Config.blurEnabled
        BackgroundEffect.radius: 4
    }
}
