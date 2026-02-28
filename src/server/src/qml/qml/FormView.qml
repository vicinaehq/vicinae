import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Flickable {
    id: root
    contentWidth: width
    contentHeight: layout.implicitHeight
    clip: true
    boundsBehavior: Flickable.StopAtBounds

    default property alias contentData: layout.data
    property real padding: 16
    property real maxContentWidth: Infinity

    function focusFirst() {
        _focusFirstIn(layout)
    }

    function _focusFirstIn(item) {
        for (let i = 0; i < item.children.length; i++) {
            let child = item.children[i]
            if (child.activeFocusOnTab) {
                child.forceActiveFocus()
                return true
            }
            if (_focusFirstIn(child)) return true
        }
        return false
    }

    ScrollBar.vertical: ViciScrollBar {
        policy: root.contentHeight > root.height
                ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
    }

    ColumnLayout {
        id: layout
        width: Math.min(root.width - root.padding * 2, root.maxContentWidth)
        x: (root.width - width) / 2
        spacing: 12

        Item { implicitHeight: 12 }
    }

    Connections {
        target: root.Window.window
        function onActiveFocusItemChanged() {
            let focused = root.Window.window ? root.Window.window.activeFocusItem : null
            if (!focused) return
            _ensureVisible(focused)
        }
    }

    function _isDescendantOf(item, ancestor) {
        for (let p = item; p; p = p.parent)
            if (p === ancestor) return true
        return false
    }

    function _ensureVisible(item) {
        if (!_isDescendantOf(item, root.contentItem)) return
        let mapped = item.mapToItem(root.contentItem, 0, 0)
        let itemTop = mapped.y
        let itemBottom = itemTop + item.height
        let viewTop = root.contentY
        let viewBottom = viewTop + root.height

        if (itemTop < viewTop) {
            root.contentY = Math.max(0, itemTop - 8)
        } else if (itemBottom > viewBottom) {
            root.contentY = Math.min(root.contentHeight - root.height,
                                      itemBottom - root.height + 8)
        }
    }
}
