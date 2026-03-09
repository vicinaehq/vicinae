import QtQuick
import QtQuick.Controls

Popup {
    id: root
    anchors.centerIn: parent
    focus: true
    modal: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    onActiveFocusChanged: {
        if (!activeFocus && opened)
            close();
    }

    enter: Transition {
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

    exit: Transition {
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

    background: Rectangle {
        radius: 10
        color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g, Theme.secondaryBackground.b, 0.98)
        border.color: Theme.divider
        border.width: 1
    }

    Overlay.modal: Rectangle {
        color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.5)
    }
}
