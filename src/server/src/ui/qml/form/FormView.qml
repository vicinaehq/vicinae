pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

Flickable {
    id: root
    contentWidth: width
    contentHeight: layout.implicitHeight
    clip: true
    boundsBehavior: Flickable.StopAtBounds
    bottomMargin: root.padding + statusBarInset.value
    topMargin: root.padding + searchBarInset.value

    default property alias contentData: layout.data
    property real padding: 16
    property real maxContentWidth: Infinity

    Component.onCompleted: {
        contentY = -topMargin;
        searchBarInset.initializePosition(root);
    }

    SearchBarInset {
        id: searchBarInset
        target: root
    }

    StatusBarInset {
        id: statusBarInset
        target: root
    }

    ViciWheelHandler {
        target: root
    }

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

    ScrollBar.vertical: ViciScrollBar {
        policy: root.contentHeight + searchBarInset.value > root.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
    }

    ColumnLayout {
        id: layout
        width: Math.min(root.width - root.padding * 2, root.maxContentWidth)
        x: (root.width - width) / 2
        spacing: 12
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
        if (root.height <= 0 || !_isDescendantOf(item, root.contentItem))
            return;
        const mapped = item.mapToItem(root.contentItem, 0, 0);
        const itemTop = mapped.y;
        const itemBottom = itemTop + item.height;
        const viewTop = root.contentY + searchBarInset.value;
        const viewBottom = root.contentY + root.height - statusBarInset.value;
        const minY = -root.topMargin;
        const maxY = Math.max(minY, root.contentHeight - root.height + root.bottomMargin);
        const gap = root.padding * 2;

        if (itemTop < viewTop) {
            root.contentY = Math.max(minY, itemTop - searchBarInset.value - gap);
        } else if (itemBottom > viewBottom) {
            root.contentY = Math.min(maxY, itemBottom - root.height + statusBarInset.value + gap);
        }
    }
}
