import QtQuick

// Resets HoverActivation when target becomes visible (covers in-scene popovers
// and transient popup-windows, which the global Show filter skips on purpose).
// Also fires on completion for targets created already-visible, e.g. a panel
// pushed onto a StackView.
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
