pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Vicinae

// Layout uses the unobscured page area. Only the scrolling surface extends
// behind the header; fixed siblings and nested scrollers keep their own bounds.
Item {
    id: root
    required property Flickable flickable
    property T.ScrollBar verticalScrollBar: flickable.T.ScrollBar.vertical
    default property alias content: surface.data
    property real topPadding: 0
    property real bottomPadding: 0
    property bool resetOnInitialization: true
    property bool _positioned: false
    signal initialized

    readonly property real topInset: {
        let top = 0;
        for (let item = root; item; item = item.parent) {
            const page = item as LauncherPage;
            if (page)
                return Math.abs(top - page.topInset) < 0.5 ? page.topInset : 0;
            if (item !== root && (item instanceof Flickable || item instanceof ScrollViewport))
                break;
            top += item.y;
        }
        return 0;
    }
    readonly property real bottomInset: statusBarInset.value
    readonly property real usableHeight: Math.max(0, flickable.height - topInset - bottomInset)
    readonly property real minimumY: flickable.originY - flickable.topMargin
    readonly property real maximumY: Math.max(minimumY, flickable.originY + flickable.contentHeight - flickable.height + flickable.bottomMargin)
    readonly property bool scrollable: flickable.contentHeight > flickable.height - topInset

    Item {
        id: surface
        y: -root.topInset
        width: root.width
        height: root.height + root.topInset
    }

    StatusBarInset {
        id: statusBarInset
        target: surface
    }

    Binding {
        target: root.flickable
        property: "topMargin"
        value: root.topInset + root.topPadding
    }
    Binding {
        target: root.flickable
        property: "bottomMargin"
        value: root.bottomInset + root.bottomPadding
    }
    Binding {
        target: root.verticalScrollBar
        property: "topPadding"
        value: root.topInset
        when: target !== null
    }

    Component.onCompleted: Qt.callLater(() => {
        if (!root._positioned && root.resetOnInitialization)
            root.resetPosition();
        root.initialized();
    })

    function scrollTo(y: real) {
        _positioned = true;
        flickable.contentY = Math.max(minimumY, Math.min(maximumY, y));
    }

    function resetPosition() {
        scrollTo(minimumY);
    }

    function revealRect(top, bottom, gap = 0) {
        if (top < flickable.contentY + topInset)
            scrollTo(top - topInset - Math.max(topPadding, gap));
        if (bottom > flickable.contentY + flickable.height - bottomInset)
            scrollTo(bottom - flickable.height + bottomInset + Math.max(bottomPadding, gap));
    }
}
