import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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

    property var _currentShortcutTokens: []
    property string _statusText: "Recording..."
    property color _statusColor: Theme.foreground

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

        _currentShortcutTokens = [];
        _statusText = "Recording...";
        _statusColor = Theme.foreground;
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

    onOpened: {
        GlobalShortcuts.setCapturing(true);
        keyReceiver.forceActiveFocus();
    }
    onAboutToHide: {
        _justClosed = true;
        reopenGuard.restart();
    }
    onClosed: GlobalShortcuts.setCapturing(false)
    onActiveFocusChanged: if (!activeFocus && opened)
        close()

    Component.onDestruction: GlobalShortcuts.setCapturing(false)

    background: Rectangle {
        readonly property bool csd: recorder.popupType === Popup.Item || Platform.supports("clientSideDecorations")
        readonly property real bgOpacity: recorder.popupType === Popup.Window ? Config.popupOpacity : 1
        radius: csd ? Math.min(Config.borderRounding, 15) : 0
        color: Qt.rgba(Theme.popoverBackground.r, Theme.popoverBackground.g, Theme.popoverBackground.b, bgOpacity)
        border.color: Config.withAlpha(Theme.popoverBorder, bgOpacity)
        border.width: csd ? 1 : 0
        PopupMaterial {}
    }

    contentItem: FocusScope {
        focus: true

        ShortcutInhibitor.enabled: recorder.opened

        Keys.onPressed: event => {
            event.accepted = true;
            closeTimer.stop();

            var key = Keyboard.normalizeKey(event.key);
            var mods = event.modifiers;

            var isModKey = key === Qt.Key_Shift || key === Qt.Key_Control || key === Qt.Key_Alt || key === Qt.Key_Meta;
            var isCloseKey = key === Qt.Key_Escape || key === Qt.Key_Backspace;

            if (!isModKey && isCloseKey && mods === Qt.NoModifier) {
                recorder.close();
                return;
            }

            if (recorder.shortcutDisplayProvider)
                recorder._currentShortcutTokens = recorder.shortcutDisplayProvider(key, mods);

            if (isModKey) {
                recorder._statusText = "Recording...";
                recorder._statusColor = Theme.foreground;
                return;
            }

            if (recorder.validateShortcut) {
                var error = recorder.validateShortcut(key, mods);
                if (error !== "") {
                    recorder._statusText = error;
                    recorder._statusColor = Theme.danger;
                    return;
                }
            }

            recorder._statusText = "Keybind updated";
            recorder._statusColor = Theme.toastSuccess;
            closeTimer.start();
            recorder.shortcutCaptured(key, mods);
        }

        Item {
            id: keyReceiver
            focus: true
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 5

            Item {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: badge.width
                Layout.preferredHeight: badge.height
                visible: recorder._currentShortcutTokens.length > 0

                ShortcutBadge {
                    id: badge
                    tokens: recorder._currentShortcutTokens
                }
            }

            Text {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                text: recorder._statusText
                color: recorder._statusColor
                font.pointSize: Theme.smallerFontSize
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
            }
        }
    }
}
