import QtQuick
import QtQuick.Controls

ScrollBar {
    id: control

    property bool _recentlyScrolled: false

    function revealOnScroll() {
        _recentlyScrolled = true;
        scrollActivityTimer.restart();
    }

    onPositionChanged: revealOnScroll()

    Timer {
        id: scrollActivityTimer
        interval: 400
        onTriggered: control._recentlyScrolled = false
    }

    contentItem: Rectangle {
        implicitWidth: 6
        implicitHeight: 6
        radius: 3
        color: Theme.scrollBarBackground
        opacity: control.active || control._recentlyScrolled ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                duration: 200
            }
        }
    }

    background: Item {}
}
