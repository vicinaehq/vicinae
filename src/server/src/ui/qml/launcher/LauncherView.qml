pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root

    property Item header: null
    property Item footer: null
    readonly property LauncherAppearance appearance: (root.Window.window as LauncherWindow)?.appearance ?? fallbackAppearance
    readonly property real topInset: {
        for (let item = root.parent; item; item = item.parent) {
            const page = item as LauncherPage;
            if (page)
                return page.topInset;
        }
        return 0;
    }
    readonly property real bottomInset: statusBarInset.value

    StatusBarInset {
        id: statusBarInset
        target: root
    }

    LauncherAppearance {
        id: fallbackAppearance
    }
}
