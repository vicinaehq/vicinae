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
        radius: Platform.supports("clientSideDecorations") ? Math.min(Config.borderRounding, 15) : 0
        color: Qt.rgba(Theme.popoverBackground.r, Theme.popoverBackground.g, Theme.popoverBackground.b, Config.windowOpacity)
        border.color: Config.withAlpha(Theme.popoverBorder, Config.windowOpacity)
        border.width: Platform.supports("clientSideDecorations") ? 1 : 0
        PopupMaterial {}
    }
}
