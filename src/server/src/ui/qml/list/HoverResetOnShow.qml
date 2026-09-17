pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Connections {
    ignoreUnknownSignals: true
    function onVisibleChanged() {
        const item = target as Item;
        if (item && item.visible)
            HoverActivation.reset();
    }
    Component.onCompleted: {
        const item = target as Item;
        if (item && item.visible)
            HoverActivation.reset();
    }
}
