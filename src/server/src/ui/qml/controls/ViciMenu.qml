pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

Menu {
    id: root

    popupType: Platform.preferItemPopup("menu") ? Popup.Item : Popup.Native
    padding: 4
    implicitWidth: Math.max(160, contentWidth + leftPadding + rightPadding)

    enter: Transition {
        NumberAnimation {
            property: "opacity"
            from: 0
            to: 1
            duration: 120
            easing.type: Easing.OutCubic
        }
    }

    exit: Transition {
        NumberAnimation {
            property: "opacity"
            from: 1
            to: 0
            duration: 80
            easing.type: Easing.InCubic
        }
    }

    background: PopoverBackground {
        popup: root
    }
}
