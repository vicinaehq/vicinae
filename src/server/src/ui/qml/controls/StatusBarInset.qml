import QtQuick
import QtQuick.Window

/// Measures how much of `target`'s bottom edge is covered by the window's
/// floating status bar. Bind the result to a Flickable's bottomMargin so
/// content can scroll clear of the glass. Yields 0 when the bar is hidden or
/// the target already ends above it.
Item {
    property Item target: parent
    visible: false

    readonly property real value: {
        const win = Window.window;
        if (!win || !(win.statusBarOverlap > 0) || !target || target.height <= 0)
            return 0;
        const bottom = target.mapToItem(null, 0, target.height).y;
        return Math.max(0, Math.min(win.statusBarOverlap, bottom - win.statusBarTop));
    }
}
