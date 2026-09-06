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

    readonly property int anchorGap: 10
    readonly property int windowEdgeMargin: 8

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

        const overlay = recorder.Overlay.overlay;
        if (recorder.popupType === Popup.Item && overlay) {
            const pad = (capture.hostWindow?.shadowPadding ?? 0) + recorder.windowEdgeMargin;
            const pos = targetItem.mapToItem(null, 0, 0);
            const spaceBelow = overlay.height - pad - (pos.y + targetItem.height + recorder.anchorGap);
            const spaceAbove = pos.y - recorder.anchorGap - pad;
            const preferred = recorder._below ? spaceBelow : spaceAbove;
            const other = recorder._below ? spaceAbove : spaceBelow;
            if (preferred < recorder.height && other > preferred)
                recorder._below = !recorder._below;
            const sceneX = Math.max(pad, Math.min(pos.x + recorder.x, overlay.width - pad - recorder.width));
            recorder.x = sceneX - pos.x;
        }

        recorder.y = recorder._below ? targetItem.height + recorder.anchorGap : -recorder.height - recorder.anchorGap;
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

    background: PopoverBackground {
        popup: recorder
        PopupMaterial {}
    }

    contentItem: ShortcutRecorderCapture {
        id: capture
        readonly property var hostWindow: Window.window
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
