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

    background: PopoverBackground {
        popup: root
        PopupMaterial {}
    }
}
