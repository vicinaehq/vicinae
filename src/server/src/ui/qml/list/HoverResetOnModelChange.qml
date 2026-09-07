import QtQuick

Connections {
    ignoreUnknownSignals: true
    function onModelReset() {
        HoverActivation.reset();
    }
    function onRowsInserted() {
        HoverActivation.reset();
    }
    function onRowsRemoved() {
        HoverActivation.reset();
    }
    function onRowsMoved() {
        HoverActivation.reset();
    }
    function onLayoutChanged() {
        HoverActivation.reset();
    }
}
