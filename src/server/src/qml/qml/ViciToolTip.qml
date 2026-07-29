import QtQuick
import QtQuick.Controls

ToolTip {
    id: root
    delay: 500
    popupType: Platform.preferItemPopup("tooltip") ? Popup.Item : Popup.Window
    // Centered above the hovered item; Qt's native ToolTip placement would
    // put it at the item's bottom-right corner instead.
    PopupPlacement.alignment: Qt.AlignHCenter | Qt.AlignTop

    contentItem: Text {
        text: root.text
        color: Theme.foreground
        font.pointSize: Theme.smallerFontSize
    }

    background: Rectangle {
        readonly property bool csd: root.popupType === Popup.Item || Platform.supports("clientSideDecorations")
        readonly property real bgOpacity: root.popupType === Popup.Window ? Config.popupOpacity : 1
        radius: csd ? Math.min(Config.borderRounding, 15) : 0
        color: Qt.rgba(Theme.popoverBackground.r, Theme.popoverBackground.g, Theme.popoverBackground.b, bgOpacity)
        border.color: Config.withAlpha(Theme.popoverBorder, bgOpacity)
        border.width: csd ? 1 : 0
        PopupMaterial {}
    }
}
