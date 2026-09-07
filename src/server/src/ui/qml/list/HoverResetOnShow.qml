import QtQuick

Connections {
    ignoreUnknownSignals: true
    function onVisibleChanged() {
        if (target && target.visible)
            HoverActivation.reset();
    }
    Component.onCompleted: {
        if (target && target.visible)
            HoverActivation.reset();
    }
}
