pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    property Item target: parent
    // typed as Window to avoid import cycle
    readonly property Window launcherWindow: root.Window.window as LauncherWindow
    property real headerHeight: launcherWindow?.searchBarOverlap ?? 0
    visible: false

    // Covered pages keep their inset when another page changes the search bar.
    Binding {
        target: root
        property: "headerHeight"
        value: root.launcherWindow?.searchBarOverlap ?? 0
        when: root.target?.visible ?? false
        restoreMode: Binding.RestoreNone
	}

    function initializePosition(flickable: Flickable) {
        if (headerHeight <= 0)
            return;
        Qt.callLater(() => {
            if (root.value <= 0)
                return;
            if (flickable instanceof ListView)
                flickable.forceLayout();
            flickable.contentY = flickable.originY - flickable.topMargin;
        });
    }

    readonly property real value: {
        if (!launcherWindow || !target || headerHeight <= 0)
            return 0;
        target.y;
        target.height;
        const top = target.mapToItem(null, 0, 0).y - launcherWindow.shadowPadding;
        return Math.max(0, Math.min(headerHeight, headerHeight - top));
    }
}
