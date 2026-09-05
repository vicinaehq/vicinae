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

    property int _heldMods: 0
    property int _chordMods: 0
    property bool _chordConsumed: false

    readonly property var _visibleTokens: _currentShortcutTokens.length > 0 ? _currentShortcutTokens : initialTokens

    implicitWidth: layout.implicitWidth
    implicitHeight: layout.implicitHeight

    function reset() {
        _currentShortcutTokens = [];
        _statusText = qsTr("Recording...");
        _statusColor = Theme.foreground;
        _resetChord();
    }

    function _resetChord() {
        _heldMods = 0;
        _chordMods = 0;
        _chordConsumed = false;
    }

    function _modifierForKey(key) {
        switch (key) {
        case Qt.Key_Shift:
            return Qt.ShiftModifier;
        case Qt.Key_Control:
            return Qt.ControlModifier;
        case Qt.Key_Alt:
            return Qt.AltModifier;
        case Qt.Key_Meta:
            return Qt.MetaModifier;
        }
        return 0;
    }

    ShortcutInhibitor.enabled: capture.capturing

    onCapturingChanged: GlobalShortcuts.setCapturing(capture.capturing)
    Component.onCompleted: {
        if (capturing)
            GlobalShortcuts.setCapturing(true);
    }
    Component.onDestruction: GlobalShortcuts.setCapturing(false)

    onActiveFocusChanged: if (!activeFocus)
        _resetChord()

    Keys.onShortcutOverride: event => event.accepted = capture.capturing

    Keys.onPressed: event => {
        event.accepted = true;
        capture.handleKey(Keyboard.normalizeKey(event.key), event.modifiers, true);
    }

    Keys.onReleased: event => {
        event.accepted = true;
        capture.handleKey(Keyboard.normalizeKey(event.key), event.modifiers, false);
    }

    Connections {
        target: GlobalShortcuts
        enabled: capture.capturing
        function onKeyCaptured(key, modifiers, down) {
            capture.handleKey(key, modifiers, down);
        }
    }

    function handleKey(key, mods, down) {
        const modKey = _modifierForKey(key);

        if (!down) {
            if (!modKey)
                return;
            _heldMods &= ~modKey;
            if (_heldMods !== 0)
                return;
            const chord = _chordMods;
            const consumed = _chordConsumed;
            _resetChord();
            if (!consumed && chord !== 0)
                _commit(key, chord & ~modKey);
            return;
        }

        capture.activity();

        if (modKey) {
            _heldMods |= modKey;
            _chordMods |= modKey;
            if (capture.shortcutDisplayProvider)
                capture._currentShortcutTokens = capture.shortcutDisplayProvider(key, _chordMods & ~modKey);
            capture._statusText = qsTr("Recording...");
            capture._statusColor = Theme.foreground;
            return;
        }

        if (_heldMods !== 0)
            _chordConsumed = true;

        const isDismissKey = key === Qt.Key_Escape || key === Qt.Key_Backspace;
        if (isDismissKey && mods === Qt.NoModifier) {
            capture.dismissRequested(key);
            return;
        }

        _commit(key, mods);
    }

    function _commit(key, mods) {
        if (capture.shortcutDisplayProvider)
            capture._currentShortcutTokens = capture.shortcutDisplayProvider(key, mods);

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
