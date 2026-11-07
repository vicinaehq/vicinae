pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

Menu {
    id: root

    popupType: Platform.preferItemPopup("menu") ? Popup.Item : Popup.Native
    padding: 4
    margins: 8
    implicitWidth: Math.max(160, _widestItem + leftPadding + rightPadding)

    readonly property real _widestItem: {
        let widest = 0;
        for (let i = 0; i < count; ++i)
            widest = Math.max(widest, itemAt(i)?.implicitWidth ?? 0);
        return widest;
    }

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
