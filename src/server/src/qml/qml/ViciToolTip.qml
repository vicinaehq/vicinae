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

    background: SourceBlendRect {
        radius: Platform.supports("clientSideDecorations") ? Math.min(Config.borderRounding, 15) : 0
        backgroundColor: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
        color: Config.withAlpha(Theme.secondaryBackground, Config.windowOpacity)
        borderColor: Config.withAlpha(Theme.divider, Config.windowOpacity)
        borderWidth: Platform.supports("clientSideDecorations") ? 1 : 0
        WindowMaterial.enabled: Config.blurEnabled
        WindowMaterial.radius: Math.min(Config.borderRounding, 15)
    }
}
