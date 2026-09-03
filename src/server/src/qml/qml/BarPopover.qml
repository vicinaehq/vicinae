import QtQuick
import QtQuick.Controls

/// Native popup hanging under a bar widget; the compositor keeps it on screen.
Popup {
    id: root

    required property Item anchorItem
    property real minimumWidth: 200

    parent: anchorItem
    popupType: Popup.Window
    modal: false
    dim: false
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    padding: 6
    y: anchorItem ? anchorItem.height + 6 : 0
    x: anchorItem ? Math.round((anchorItem.width - width) / 2) : 0
    PopupPlacement.alignment: Qt.AlignHCenter | Qt.AlignBottom

    background: Rectangle {
        radius: Math.min(Config.borderRounding, 12)
        color: Config.withAlpha(Theme.popoverBackground, Config.popupOpacity)
        border.color: Config.withAlpha(Theme.popoverBorder, Config.popupOpacity)
        border.width: 1

        PopupMaterial {}
    }
}
