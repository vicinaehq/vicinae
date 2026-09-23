pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

FocusScope {
    id: root

    readonly property alias inputItem: editor.inputItem
    property alias text: editor.text
    property alias placeholder: editor.placeholder
    property var modelItems: []
    property var currentModel: null
    property bool busy: false
    property int maxRows: 6
    property bool dictationAvailable: false
    property bool recording: false
    property bool transcribing: false
    property string recordingTime: ""
    property string dictationMessage: ""

    signal submitted(string text)
    signal cancelled
    signal modelActivated(var item)
    signal dictationToggled
    signal dictationCancelled

    readonly property bool canSubmit: !busy && editor.text.trim().length > 0
    readonly property real lineHeight: Math.ceil(fontMetrics.height)
    readonly property real _verticalPadding: 9
    readonly property real _leftPadding: 18
    readonly property real _rightPadding: 8
    readonly property real _controlHeight: 28
    readonly property real _spacing: 6
    readonly property real _editorHeight: Math.max(lineHeight, Math.min(editor.contentHeight, lineHeight * maxRows))
    readonly property real _inlineEditorWidth: width - _leftPadding - _rightPadding - controls.width - _spacing
    readonly property bool expanded: editor.text.length > 0 && (editor.text.includes("\n") || textMetrics.advanceWidth > _inlineEditorWidth)
    implicitHeight: (expanded ? _editorHeight + _spacing + _controlHeight : Math.max(_editorHeight, _controlHeight)) + _verticalPadding * 2

    function forceActiveFocus() {
        editor.forceActiveFocus();
    }

    function submit() {
        if (!root.canSubmit)
            return;
        const text = editor.text.trim();
        editor.text = "";
        root.submitted(text);
    }

    function insertText(text) {
        const current = editor.text;
        const needsSpace = current.length > 0 && !/\s$/.test(current);
        editor.text = current + (needsSpace ? " " : "") + text;
        editor.cursorPosition = editor.text.length;
        editor.forceActiveFocus();
    }

    Keys.onEscapePressed: event => {
        if (!root.recording) {
            event.accepted = false;
            return;
        }
        root.dictationCancelled();
    }

    Shortcut {
        sequence: Keybinds.openSearchAccessorySequence
        enabled: root.visible
        onActivated: modelSelector.open()
    }

    FontMetrics {
        id: fontMetrics
        font: editor.font
    }

    TextMetrics {
        id: textMetrics
        font: editor.font
        text: editor.text
    }

    FormInputFrame {
        anchors.fill: parent
        filled: true
        focused: editor.editing
        radius: Math.min(height / 2, 24)
        opaque: true
    }

    FormTextEditor {
        id: editor
        x: root._leftPadding
        y: root._verticalPadding + (root.expanded ? 0 : (Math.max(root._editorHeight, root._controlHeight) - root._editorHeight) / 2)
        width: root.expanded ? root.width - root._leftPadding * 2 : root._inlineEditorWidth
        height: root._editorHeight
        multiline: true
        submitOnReturn: true
        onAccepted: root.submit()
    }

    Row {
        id: controls
        anchors.right: parent.right
        anchors.rightMargin: root._rightPadding
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root._verticalPadding
        height: root._controlHeight
        spacing: root._spacing

        SearchableDropdown {
            id: modelSelector
            anchors.verticalCenter: parent.verticalCenter
            visible: !root.recording && !root.transcribing
            compact: true
            flat: true
            preferredPopupEdge: Qt.TopEdge
            popupAnchor: root
            items: root.modelItems
            currentItem: root.currentModel
            onActivated: item => root.modelActivated(item)
            onPopupClosed: editor.forceActiveFocus()
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            visible: root.recording || root.transcribing || root.dictationMessage !== ""
            text: root.dictationMessage !== "" ? root.dictationMessage : (root.transcribing ? qsTr("Transcribing…") : root.recordingTime)
            color: root.dictationMessage !== "" ? Theme.danger : Theme.textMuted
            font.family: Theme.fontFamily
            font.pointSize: Theme.smallerFontSize
        }

        Item {
            anchors.verticalCenter: parent.verticalCenter
            width: root._controlHeight
            height: root._controlHeight
            visible: root.transcribing

            PulsingDots {
                anchors.centerIn: parent
                active: parent.visible
            }
        }

        Rectangle {
            id: micButton
            anchors.verticalCenter: parent.verticalCenter
            visible: !root.transcribing
            width: root._controlHeight
            height: root._controlHeight
            radius: root._controlHeight / 2
            enabled: root.dictationAvailable
            opacity: enabled ? 1.0 : 0.4
            color: root.recording ? Config.withAlpha(Theme.danger, micHover.hovered ? 0.25 : 0.15) : (micHover.hovered ? Theme.listItemHoverBg : "transparent")

            ViciImage {
                anchors.centerIn: parent
                width: 14
                height: 14
                source: Img.icon(root.recording ? BuiltinIcon.Stop : BuiltinIcon.Microphone).withFillColor(root.recording ? Theme.danger : Theme.textMuted)
            }

            HoverHandler {
                id: micHover
                enabled: micButton.enabled
                cursorShape: Qt.PointingHandCursor
            }

            TapHandler {
                enabled: micButton.enabled
                onTapped: root.dictationToggled()
            }

            ToolTip.visible: micHover.hovered
            ToolTip.text: root.recording ? qsTr("Stop and transcribe") : (root.dictationAvailable ? qsTr("Dictate") : qsTr("Set up dictation to talk to Quick AI"))
            ToolTip.delay: 600
        }

        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width: root._controlHeight
            height: root._controlHeight
            radius: root._controlHeight / 2
            color: root.busy || root.canSubmit ? Theme.foreground : Config.withAlpha(Theme.foreground, 0.25)

            ViciImage {
                anchors.centerIn: parent
                width: 14
                height: 14
                source: Img.icon(root.busy ? BuiltinIcon.Stop : BuiltinIcon.ArrowUp).withFillColor(Theme.background)
            }

            HoverHandler {
                cursorShape: root.busy || root.canSubmit ? Qt.PointingHandCursor : Qt.ArrowCursor
            }

            TapHandler {
                onTapped: root.busy ? root.cancelled() : root.submit()
            }
        }
    }
}
