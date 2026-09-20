pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import Vicinae

FocusScope {
    id: root

    property alias text: editor.text
    property alias placeholder: editor.placeholder
    property var modelItems: []
    property var currentModel: null
    property bool busy: false
    property int maxRows: 6
    property Item backdrop: null
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
    readonly property bool expanded: editor.text.includes("\n") || textMetrics.advanceWidth > _inlineEditorWidth
    readonly property real _radius: Math.min(height / 2, 24)
    readonly property bool _frosted: backdrop !== null
    readonly property int _backdropPad: 96

    implicitHeight: (expanded ? _editorHeight + _spacing + _controlHeight : Math.max(_editorHeight, _controlHeight)) + _verticalPadding * 2

    readonly property point _origin: {
        root.x;
        root.y;
        root.width;
        root.height;
        return root.backdrop ? root.mapToItem(root.backdrop, 0, 0) : Qt.point(0, 0);
    }
    readonly property rect _captureRect: {
        if (!root.backdrop)
            return Qt.rect(0, 0, 1, 1);
        const x0 = Math.max(0, _origin.x - _backdropPad);
        const y0 = Math.max(0, _origin.y - _backdropPad);
        const x1 = Math.min(root.backdrop.width, _origin.x + root.width + _backdropPad);
        const y1 = Math.min(root.backdrop.height, _origin.y + root.height + _backdropPad);
        return Qt.rect(x0, y0, Math.max(1, x1 - x0), Math.max(1, y1 - y0));
    }

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

    Rectangle {
        visible: root._frosted
        anchors.fill: parent
        radius: root._radius
        color: Theme.background
    }

    ShaderEffectSource {
        id: backdropSource
        visible: false
        live: true
        sourceItem: root._frosted ? root.backdrop : null
        sourceRect: root._captureRect
        textureSize: Qt.size(Math.max(1, Math.round(root._captureRect.width / 16)), Math.max(1, Math.round(root._captureRect.height / 16)))
    }

    MultiEffect {
        visible: root._frosted
        x: root._captureRect.x - root._origin.x
        y: root._captureRect.y - root._origin.y
        width: root._captureRect.width
        height: root._captureRect.height
        source: backdropSource
        autoPaddingEnabled: false
        blurEnabled: true
        blur: 1.0
        blurMax: 64

        layer.enabled: root._frosted
        layer.effect: MultiEffect {
            autoPaddingEnabled: false
            blurEnabled: true
            blur: 1.0
            blurMax: 64
            maskEnabled: true
            maskSource: blurMask
        }
    }

    Item {
        id: blurMask
        width: root._captureRect.width
        height: root._captureRect.height
        visible: false
        layer.enabled: true

        Rectangle {
            x: root._origin.x - root._captureRect.x
            y: root._origin.y - root._captureRect.y
            width: root.width
            height: root.height
            radius: root._radius
            color: "white"
        }
    }

    Rectangle {
        anchors.fill: parent
        radius: root._radius
        color: Config.withAlpha(Theme.inputBackground, (root._frosted ? 0.78 : 1.0) * Config.surfaceOpacity)
        border.width: 1
        border.color: Config.withAlpha(editor.editing ? Theme.inputBorderFocus : Theme.inputBorder, Config.surfaceOpacity)
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
