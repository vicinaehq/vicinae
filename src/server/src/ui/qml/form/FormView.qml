pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae
import Vicinae.Scrolling as Scrolling

ScrollViewport {
    id: root
    flickable: flick
    topPadding: padding
    bottomPadding: padding

    default property alias contentData: layout.data
    property real padding: 16
    property real maxContentWidth: Infinity

    function focusFirst() {
        _focusFirstIn(layout);
    }

    function _focusFirstIn(item) {
        for (let i = 0; i < item.children.length; i++) {
            let child = item.children[i];
            if (child.activeFocusOnTab) {
                child.forceActiveFocus();
                return true;
            }
            if (_focusFirstIn(child))
                return true;
        }
        return false;
    }

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: width
        contentHeight: layout.implicitHeight
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        Scrolling.WheelHandler {
            target: flick
        }

        ScrollBar.vertical: ViciScrollBar {
            policy: root.scrollable ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
        }

        ColumnLayout {
            id: layout
            width: Math.min(root.width - root.padding * 2, root.maxContentWidth)
            x: (root.width - width) / 2
            spacing: 12
        }
    }

    Connections {
        target: root.Window.window
        function onActiveFocusItemChanged() {
            let focused = root.Window.window ? root.Window.window.activeFocusItem : null;
            if (!focused)
                return;
            root._ensureVisible(focused);
        }
    }

    function _isDescendantOf(item, ancestor) {
        for (let p = item; p; p = p.parent)
            if (p === ancestor)
                return true;
        return false;
    }

    function _ensureVisible(item) {
        if (root.height <= 0 || !_isDescendantOf(item, flick.contentItem))
            return;
        const mapped = item.mapToItem(flick.contentItem, 0, 0);
        root.revealRect(mapped.y, mapped.y + item.height, root.padding * 2);
    }
}
