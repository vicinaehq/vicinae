import QtQuick
import Vicinae

// Qt restarts the blink cycle on keystrokes for TextInput but not for TextEdit
Rectangle {
    id: cursor
    width: 1
    color: Theme.foreground

    readonly property TextEdit _edit: parent as TextEdit
    readonly property int _flashTime: Application.styleHints.cursorFlashTime

    visible: _edit?.cursorVisible ?? false

    function reset() {
        blink.stop();
        opacity = 1;
        if (visible && _flashTime > 0)
            blink.start();
    }

    Component.onCompleted: reset()
    onVisibleChanged: reset()

    SequentialAnimation {
        id: blink
        loops: Animation.Infinite
        PauseAnimation {
            duration: cursor._flashTime / 2
        }
        PropertyAction {
            target: cursor
            property: "opacity"
            value: 0
        }
        PauseAnimation {
            duration: cursor._flashTime / 2
        }
        PropertyAction {
            target: cursor
            property: "opacity"
            value: 1
        }
    }

    Connections {
        target: cursor._edit
        function onCursorPositionChanged() {
            cursor.reset();
        }
    }
}
