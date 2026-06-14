import QtQuick
import QtQuick.Layouts

// Reusable shortcut recorder field. Works with serialized shortcut strings; key/modifier
// conversion is delegated to the `Keyboard` bridge so callers don't reimplement it.
RowLayout {
    id: field

    // Current shortcut in serialized form (e.g. "super+shift+space"); empty if unset.
    property string shortcut: ""
    // Whether to draw an outline around the trigger (off for dense list rows).
    property bool bordered: true
    property string placeholder: "Record shortcut"
    // Owner id of this shortcut, excluded from conflict checks (so re-recording it isn't self-conflict).
    property string shortcutId: ""

    signal accepted(string shortcut)
    signal cleared

    readonly property var _tokens: Keyboard.tokensForString(shortcut)

    // The recorder is a window-backed Popup; create it lazily so dense lists
    // (e.g. hundreds of command rows) don't each build one upfront.
    property var _recorder: null
    function _showRecorder() {
        if (!_recorder)
            _recorder = recorderComponent.createObject(trigger);
        if (_recorder)
            _recorder.show(trigger);
    }

    spacing: 4

    Rectangle {
        id: trigger
        Layout.preferredHeight: 26
        implicitWidth: Math.max(content.implicitWidth + 16, 80)
        radius: 4
        color: "transparent"
        // Borderless fields (dense list rows) still reveal a border on hover.
        border.width: hover.hovered || (field.bordered && field._tokens.length > 0) ? 1 : 0
        border.color: Config.withAlpha(Theme.inputBorder, Config.windowOpacity)

        HoverHandler {
            id: hover
        }
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: field._showRecorder()
        }

        Item {
            id: content
            anchors.centerIn: parent
            implicitWidth: field._tokens.length > 0 ? badge.width : placeholderText.width
            implicitHeight: 18

            ShortcutBadge {
                id: badge
                anchors.centerIn: parent
                visible: field._tokens.length > 0
                tokens: field._tokens
            }

            Text {
                id: placeholderText
                anchors.centerIn: parent
                visible: field._tokens.length === 0
                text: field.placeholder
                color: Theme.textPlaceholder
                font.pointSize: Theme.smallerFontSize
            }
        }
    }

    Text {
        visible: field._tokens.length > 0
        text: "×"
        color: clearArea.containsMouse ? Theme.danger : Theme.textMuted
        font.pixelSize: 16

        MouseArea {
            id: clearArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: field.cleared()
        }
    }

    Component {
        id: recorderComponent
        ShortcutRecorderField {
            shortcutDisplayProvider: (key, mods) => Keyboard.tokens(key, mods)
            validateShortcut: (key, mods) => GlobalShortcuts.validate(key, mods, field.shortcutId)
            onShortcutCaptured: (key, modifiers) => {
                const serialized = Keyboard.serialize(key, modifiers);
                if (serialized !== "")
                    field.accepted(serialized);
            }
        }
    }
}
