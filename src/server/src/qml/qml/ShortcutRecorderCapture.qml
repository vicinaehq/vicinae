import QtQuick
import QtQuick.Layouts

// Shared key-capture core for shortcut recorders: key normalization, validation,
// status feedback, and the capture lifecycle (global shortcut suspension +
// compositor shortcut inhibition). Hosts decide where to embed it and when to close.
FocusScope {
    id: capture

    // Returns a user-facing error string, or "" if the shortcut is acceptable.
    property var validateShortcut: null
    // Returns display tokens for a (key, modifiers) pair.
    property var shortcutDisplayProvider: null
    // Suspends global shortcuts and inhibits compositor ones while true.
    property bool capturing: false
    // Tokens shown before the first keypress (e.g. the currently assigned shortcut).
    property var initialTokens: []

    signal shortcutCaptured(int key, int modifiers)
    // Escape or Backspace pressed bare; hosts decide whether that closes or clears.
    signal dismissRequested(int key)
    // Any keypress; hosts use this to interrupt pending auto-close timers.
    signal activity

    property var _currentShortcutTokens: []
    property string _statusText: qsTr("Recording...")
    property color _statusColor: Theme.foreground

    readonly property var _visibleTokens: _currentShortcutTokens.length > 0 ? _currentShortcutTokens : initialTokens

    implicitWidth: layout.implicitWidth
    implicitHeight: layout.implicitHeight

    function reset() {
        _currentShortcutTokens = [];
        _statusText = qsTr("Recording...");
        _statusColor = Theme.foreground;
    }

    ShortcutInhibitor.enabled: capture.capturing

    onCapturingChanged: GlobalShortcuts.setCapturing(capture.capturing)
    Component.onCompleted: {
        if (capturing)
            GlobalShortcuts.setCapturing(true);
    }
    Component.onDestruction: GlobalShortcuts.setCapturing(false)

    Keys.onShortcutOverride: event => event.accepted = capture.capturing

    Keys.onPressed: event => {
        event.accepted = true;
        capture.activity();

        const key = Keyboard.normalizeKey(event.key);
        const mods = event.modifiers;

        const isModKey = key === Qt.Key_Shift || key === Qt.Key_Control || key === Qt.Key_Alt || key === Qt.Key_Meta;
        const isDismissKey = key === Qt.Key_Escape || key === Qt.Key_Backspace;

        if (!isModKey && isDismissKey && mods === Qt.NoModifier) {
            capture.dismissRequested(key);
            return;
        }

        if (capture.shortcutDisplayProvider)
            capture._currentShortcutTokens = capture.shortcutDisplayProvider(key, mods);

        if (isModKey) {
            capture._statusText = qsTr("Recording...");
            capture._statusColor = Theme.foreground;
            return;
        }

        if (capture.validateShortcut) {
            const error = capture.validateShortcut(key, mods);
            if (error !== "") {
                capture._statusText = error;
                capture._statusColor = Theme.danger;
                return;
            }
        }

        capture._statusText = qsTr("Keybind updated");
        capture._statusColor = Theme.toastSuccess;
        capture.shortcutCaptured(key, mods);
    }

    Item {
        id: keyReceiver
        focus: true
    }

    ColumnLayout {
        id: layout
        anchors.fill: parent
        spacing: 5

        Item {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: badge.width
            Layout.preferredHeight: badge.height
            visible: capture._visibleTokens.length > 0

            ShortcutBadge {
                id: badge
                tokens: capture._visibleTokens
            }
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            text: capture._statusText
            color: capture._statusColor
            font.pointSize: Theme.smallerFontSize
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
        }
    }
}
