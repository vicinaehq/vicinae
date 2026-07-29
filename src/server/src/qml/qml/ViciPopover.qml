import QtQuick
import QtQuick.Controls

Popup {
    id: root

    property string surface: "popover"
    property bool nativeWindow: !Platform.preferItemPopup(surface)
    property bool nativeAnimationEnabled: true
    property bool itemAnimationEnabled: true
    property real animationAnchorX: 0.5
    property real animationAnchorY: 0.5
    property real backgroundOpacity: isNativeWindow ? Config.popupOpacity : 1

    readonly property alias popupMaterial: materialImpl
    readonly property bool isNativeWindow: popupType === Popup.Window
    readonly property bool hasNativeAnimation: nativeAnimationEnabled && isNativeWindow && popupMaterial.macImpl !== null

    popupType: nativeWindow ? Popup.Window : Popup.Item
    padding: 1

    onAboutToShow: if (popupMaterial.macImpl)
        popupMaterial.macImpl.animateIn(root.animationAnchorX, root.animationAnchorY)
    onAboutToHide: if (popupMaterial.macImpl)
        popupMaterial.macImpl.animateOut(root.animationAnchorX, root.animationAnchorY)

    enter: root.isNativeWindow || !root.itemAnimationEnabled ? null : _itemEnter
    exit: root.hasNativeAnimation ? _holdExit : (root.isNativeWindow || !root.itemAnimationEnabled ? null : _itemExit)

    property Transition _itemEnter: Transition {
        ParallelAnimation {
            NumberAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: 150
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                property: "scale"
                from: 0.95
                to: 1
                duration: 150
                easing.type: Easing.OutCubic
            }
        }
    }

    property Transition _itemExit: Transition {
        ParallelAnimation {
            NumberAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: 100
                easing.type: Easing.InCubic
            }
            NumberAnimation {
                property: "scale"
                from: 1
                to: 0.95
                duration: 100
                easing.type: Easing.InCubic
            }
        }
    }

    property Transition _holdExit: Transition {
        PauseAnimation {
            duration: 110
        }
    }

    background: Rectangle {
        readonly property bool csd: !root.isNativeWindow || Platform.supports("clientSideDecorations")
        radius: csd ? Math.min(Config.borderRounding, 15) : 0
        color: Qt.rgba(Theme.popoverBackground.r, Theme.popoverBackground.g, Theme.popoverBackground.b, root.backgroundOpacity)
        border.color: Config.withAlpha(Theme.popoverBorder, root.backgroundOpacity)
        border.width: csd ? 1 : 0

        PopupMaterial {
            id: materialImpl
        }
    }
}
