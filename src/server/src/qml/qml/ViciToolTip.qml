import QtQuick
import QtQuick.Controls

ToolTip {
    id: root
    delay: 500
    popupType: Popup.Window
    // Centered above the hovered item; Qt's native ToolTip placement would
    // put it at the item's bottom-right corner instead.
    PopupPlacement.alignment: Qt.AlignHCenter | Qt.AlignTop

    contentItem: Text {
        text: root.text
        color: Theme.foreground
        font.pointSize: Theme.smallerFontSize
    }

    background: Rectangle {
        color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g, Theme.secondaryBackground.b, 0.95)
        border.color: Config.withAlpha(Theme.divider, Config.windowOpacity)
        border.width: 1
        radius: 4
        BackgroundEffect.enabled: Config.blurEnabled
        BackgroundEffect.radius: 4
    }
}
