import QtQuick
import QtQuick.Controls

Popup {
    id: recorder

    property var validateShortcut: null
    property var shortcutDisplayProvider: null

    signal shortcutCaptured(int key, int modifiers)

    width: 250
    height: 80
    focus: true
    closePolicy: Popup.CloseOnPressOutside
    popupType: Platform.preferItemPopup("popover") ? Popup.Item : Popup.Window
    PopupPlacement.alignment: Qt.AlignHCenter | (recorder._below ? Qt.AlignBottom : Qt.AlignTop)
    padding: 10

    property bool _below: false
    property bool _justClosed: false

    Timer {
        id: closeTimer
        interval: 2000
        onTriggered: recorder.close()
    }

    Timer {
        id: reopenGuard
        interval: 300
        onTriggered: recorder._justClosed = false
    }

    function show(targetItem, below) {
        if (_justClosed)
            return false;

        capture.reset();
        closeTimer.stop();

        // Parent to the trigger so the native popup anchors to it; x/y only
        // apply on non-Wayland platforms.
        recorder._below = !!below;
        recorder.parent = targetItem;
        recorder.x = targetItem.width / 2 - recorder.width / 2;
        recorder.y = below ? targetItem.height + 10 : -recorder.height - 10;
        recorder.open();
        return true;
    }

    onOpened: capture.forceActiveFocus()
    onAboutToHide: {
        _justClosed = true;
        reopenGuard.restart();
    }
    onActiveFocusChanged: if (!activeFocus && opened)
        close()

    background: Rectangle {
        readonly property bool csd: recorder.popupType === Popup.Item || Platform.supports("clientSideDecorations")
        readonly property real bgOpacity: recorder.popupType === Popup.Window ? Config.popupOpacity : 1
        radius: csd ? Math.min(Config.borderRounding, 15) : 0
        color: Qt.rgba(Theme.popoverBackground.r, Theme.popoverBackground.g, Theme.popoverBackground.b, bgOpacity)
        border.color: Config.withAlpha(Theme.popoverBorder, bgOpacity)
        border.width: csd ? 1 : 0
        PopupMaterial {}
    }

    contentItem: ShortcutRecorderCapture {
        id: capture
        focus: true
        capturing: recorder.opened
        validateShortcut: recorder.validateShortcut
        shortcutDisplayProvider: recorder.shortcutDisplayProvider

        onActivity: closeTimer.stop()
        onDismissRequested: recorder.close()
        onShortcutCaptured: (key, modifiers) => {
            closeTimer.start();
            recorder.shortcutCaptured(key, modifiers);
        }
    }
}
