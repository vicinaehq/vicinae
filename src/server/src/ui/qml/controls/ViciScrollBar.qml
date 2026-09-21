pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

ScrollBar {
    id: control

    property bool _recentlyScrolled: false

    bottomPadding: control.orientation === Qt.Vertical ? statusBarInset.value : 0

    StatusBarInset {
        id: statusBarInset
        target: control
    }

    function revealOnScroll() {
        if (size <= 0 || size >= 1)
            return;
        _recentlyScrolled = true;
        scrollActivityTimer.restart();
    }

    onPositionChanged: revealOnScroll()

    onSizeChanged: {
        if (size <= 0 || size >= 1) {
            _recentlyScrolled = false;
            scrollActivityTimer.stop();
        }
    }

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
        visible: control.policy === ScrollBar.AlwaysOn || (control.size > 0 && control.size < 1)
        opacity: control.policy === ScrollBar.AlwaysOn || control.active || control._recentlyScrolled ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation {
                duration: 200
            }
        }
    }

    background: Item {}
}
