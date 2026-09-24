pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

FocusScope {
    id: root

    readonly property alias inputItem: editor.inputItem
    property alias text: editor.text
    property alias placeholder: editor.placeholder
    property alias font: editor.font
    property var modelItems: []
    property var currentModel: null
    property AttachmentModel attachments: null
    property bool submissionEnabled: true
    property string message: ""
    property bool busy: false
    property bool compact: true
    property int maxRows: compact ? 6 : 10
    property bool dictationAvailable: false
    property bool recording: false
    property bool transcribing: false
    property string recordingTime: ""
    property string dictationMessage: ""

    signal submitted(string text)
    signal previewRequested(var content)
    signal cancelled
    signal modelActivated(var item)
    signal dictationToggled
    signal dictationCancelled

    readonly property bool canSubmit: !busy && submissionEnabled && (attachments?.ready ?? true) && (editor.text.trim().length > 0 || (attachments?.count ?? 0) > 0)
    readonly property real lineHeight: Math.ceil(fontMetrics.height)
    readonly property real _verticalPadding: compact ? 9 : 10
    readonly property real _leftPadding: compact ? 18 : 14
    readonly property real _rightPadding: 8
    readonly property real _controlHeight: 28
    readonly property real _spacing: 6
    readonly property real _editorHeight: Math.max(lineHeight, Math.min(editor.contentHeight, lineHeight * maxRows))
    readonly property real _inlineEditorWidth: width - _leftPadding - _rightPadding - controls.width - _spacing
    readonly property bool expanded: !compact || (editor.text.length > 0 && (editor.text.includes("\n") || textMetrics.advanceWidth > _inlineEditorWidth))
    readonly property real _attachmentHeight: attachmentList.visible ? attachmentList.implicitHeight + _spacing : 0
    readonly property real _messageHeight: notice.visible ? notice.implicitHeight + _spacing : 0
    implicitHeight: _attachmentHeight + _messageHeight + (expanded ? _editorHeight + _spacing + _controlHeight : Math.max(_editorHeight, _controlHeight)) + _verticalPadding * 2

    function forceActiveFocus() {
        editor.forceActiveFocus();
    }

    function submit() {
        if (!root.canSubmit)
            return;
        const text = editor.text.trim();
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
        radius: root.compact ? Math.min(height / 2, 24) : 14
        opaque: true
    }

    AttachmentList {
        id: attachmentList
        x: root._leftPadding
        y: root._verticalPadding
        width: root.width - root._leftPadding * 2
        visible: (root.attachments?.count ?? 0) > 0
        attachments: root.attachments?.items ?? []
        removable: true
        onRemoveRequested: attachmentId => root.attachments.remove(attachmentId)
        onPreviewRequested: content => root.previewRequested(content)
    }

    Text {
        id: notice
        x: root._leftPadding
        y: root._verticalPadding + root._attachmentHeight
        width: root.width - root._leftPadding * 2
        visible: root.message.length > 0
        text: root.message
        textFormat: Text.PlainText
        wrapMode: Text.Wrap
        color: Theme.danger
        font.family: Theme.fontFamily
        font.pointSize: Theme.smallerFontSize
    }

    AttachmentPasteHandler {
        model: root.attachments
        target: editor.inputItem
        enabled: !root.busy
    }

    FilePickerDialog {
        id: picker
        multiple: true
        onAccepted: paths => root.attachments?.addFiles(paths)
        onClosed: editor.forceActiveFocus()
    }

    FormTextEditor {
        id: editor
        x: root._leftPadding
        y: root._verticalPadding + root._attachmentHeight + root._messageHeight + (root.expanded ? 0 : (Math.max(root._editorHeight, root._controlHeight) - root._editorHeight) / 2)
        width: root.expanded ? root.width - root._leftPadding * 2 : root._inlineEditorWidth
        height: root._editorHeight
        multiline: true
        submitOnReturn: true
        onAccepted: root.submit()
    }

    AttachmentDropArea {
        anchors.fill: parent
        attachmentModel: root.attachments
        enabled: root.attachments !== null && !root.busy
        z: 1
    }

    RowLayout {
        id: controls
        anchors.right: parent.right
        anchors.rightMargin: root._rightPadding
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root._verticalPadding
        width: root.compact ? implicitWidth : root.width - root._rightPadding * 2
        height: root._controlHeight
        spacing: root._spacing

        ViciButton {
            id: attachButton
            visible: root.attachments !== null
            enabled: !root.busy
            implicitWidth: root._controlHeight
            implicitHeight: root._controlHeight
            iconSize: 16
            iconSource: Img.icon(BuiltinIcon.Paperclip).withFillColor(Theme.textMuted)
            accessibleName: qsTr("Attach images or text files")
            activeFocusOnTab: true
            onClicked: picker.open()
            ToolTip.visible: hovered
            ToolTip.text: accessibleName
            ToolTip.delay: 600
        }

        SearchableDropdown {
            id: modelSelector
            Layout.preferredWidth: Math.min(preferredWidth, root.width * 0.55)
            Layout.alignment: Qt.AlignVCenter
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

        Item {
            visible: !root.compact
            Layout.fillWidth: true
        }

        Text {
            Layout.alignment: Qt.AlignVCenter
            visible: root.recording || root.transcribing || root.dictationMessage !== ""
            text: root.dictationMessage !== "" ? root.dictationMessage : (root.transcribing ? qsTr("Transcribing…") : root.recordingTime)
            color: root.dictationMessage !== "" ? Theme.danger : Theme.textMuted
            font.family: Theme.fontFamily
            font.pointSize: Theme.smallerFontSize
        }

        Item {
            Layout.alignment: Qt.AlignVCenter
            implicitWidth: root._controlHeight
            implicitHeight: root._controlHeight
            visible: root.transcribing

            PulsingDots {
                anchors.centerIn: parent
                active: parent.visible
            }
        }

        ViciButton {
            Layout.alignment: Qt.AlignVCenter
            visible: !root.transcribing
            implicitWidth: root._controlHeight
            implicitHeight: root._controlHeight
            radius: root._controlHeight / 2
            enabled: root.dictationAvailable
            opacity: enabled ? 1.0 : 0.4
            color: root.recording ? Config.withAlpha(Theme.danger, hovered ? 0.25 : 0.15) : (hovered ? Theme.listItemHoverBg : "transparent")
            iconSize: 14
            iconSource: Img.icon(root.recording ? BuiltinIcon.Stop : BuiltinIcon.Microphone).withFillColor(root.recording ? Theme.danger : Theme.textMuted)
            accessibleName: root.recording ? qsTr("Stop and transcribe") : qsTr("Dictate")
            activeFocusOnTab: true
            onClicked: root.dictationToggled()
            ToolTip.visible: hovered
            ToolTip.text: root.dictationAvailable ? accessibleName : qsTr("Set up dictation to talk to AI")
            ToolTip.delay: 600
        }

        ViciButton {
            Layout.alignment: Qt.AlignVCenter
            implicitWidth: root._controlHeight
            implicitHeight: root._controlHeight
            radius: root._controlHeight / 2
            enabled: root.busy || root.canSubmit
            color: enabled ? Theme.foreground : Config.withAlpha(Theme.foreground, 0.25)
            iconSize: 14
            iconSource: Img.icon(root.busy ? BuiltinIcon.Stop : BuiltinIcon.ArrowUp).withFillColor(Theme.background)
            accessibleName: root.busy ? qsTr("Stop response") : qsTr("Send message")
            activeFocusOnTab: true
            onClicked: root.busy ? root.cancelled() : root.submit()
            ToolTip.visible: hovered
            ToolTip.text: accessibleName
            ToolTip.delay: 600
        }
    }
}
