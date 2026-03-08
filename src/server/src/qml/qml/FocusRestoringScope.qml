import QtQuick
import QtQuick.Window

FocusScope {
    id: root
    property bool active: false
    property Item _savedFocusItem: null

    onActiveChanged: {
        if (active) {
            _savedFocusItem = Window.window ? Window.window.activeFocusItem : null;
            focus = true;
        } else {
            focus = false;
            if (_savedFocusItem) {
                _savedFocusItem.forceActiveFocus();
                _savedFocusItem = null;
            }
        }
    }
}
