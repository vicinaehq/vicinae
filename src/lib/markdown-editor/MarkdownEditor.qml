pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae.Editor

FocusScope {
    id: root
    required property MarkdownEditorDocument document
    property color foreground: "#dddddd"
    property color mutedForeground: "#888888"
    property color selectionColor: "#415672"
    property color selectedTextColor: foreground
    property color toolbarBackground: "#242428"
    property color toolbarBorder: "#45454b"
    property color toolbarHover: "#35353b"
    property var formattingIcons: ({})
    property string fontFamily: "monospace"
    property real fontSize: 14
    property real contentPadding: Math.max(32, (width - 760) / 2)
    property real topPadding: 28
    property string placeholderText: qsTr("Start writing…")
    readonly property alias selectionStart: edit.selectionStart
    readonly property alias selectionEnd: edit.selectionEnd
    readonly property alias cursorPosition: edit.cursorPosition
    readonly property alias composing: edit.inputMethodComposing
    readonly property alias flickable: viewport
    property bool readOnly: false
    property var pasteHandler: null
    property bool canPasteAttachment: false
    property bool _formattingDismissed: false

    function applyFormat(format) {
        if (root.readOnly)
            return;
        root.document.format(edit.selectionStart, edit.selectionEnd, format);
        edit.forceActiveFocus();
    }

    function focusEditor() {
        edit.forceActiveFocus();
    }
    function paste() {
        if (root.readOnly)
            return;
        if (!root.pasteHandler || !root.pasteHandler(edit.selectionStart, edit.selectionEnd))
            edit.paste();
    }
    function find(query, backwards, incremental = false) {
        const from = backwards || incremental ? edit.selectionStart : edit.selectionEnd;
        const match = document.find(query, from, backwards);
        if (match.start === undefined) {
            if (incremental)
                edit.cursorPosition = from;
            return false;
        }
        edit.select(match.start, match.end);
        ensureCursorVisible();
        return true;
    }
    function ensureCursorVisible() {
        const top = edit.y + edit.cursorRectangle.y;
        const bottom = top + edit.cursorRectangle.height;
        if (top < viewport.contentY + 12)
            viewport.contentY = Math.max(0, top - 12);
        else if (bottom > viewport.contentY + viewport.height - 24)
            viewport.contentY = Math.min(viewport.contentHeight - viewport.height, bottom - viewport.height + 24);
    }

    Binding {
        target: root.document
        property: "textDocument"
        value: edit.textDocument
    }
    // Qt hides an off-screen caret internally without restoring it when a click moves it.
    Binding {
        target: edit
        property: "cursorVisible"
        value: edit.activeFocus && edit.cursorRectangle.y + edit.y + edit.cursorRectangle.height > viewport.contentY && edit.cursorRectangle.y + edit.y < viewport.contentY + viewport.height
    }
    Connections {
        target: root.document
        function onSelectionRequested(start: int, end: int) {
            edit.select(start, end);
        }
        function onDocumentChanged() {
            edit.cursorPosition = 0;
            viewport.contentY = 0;
        }
        function onTextChanged() {
            if (formatting.hasSelection)
                Qt.callLater(formatting.refreshGeometry);
        }
    }

    Flickable {
        id: viewport
        anchors.fill: parent
        clip: true
        contentWidth: width
        contentHeight: Math.max(height, edit.y + edit.contentHeight + 64)
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick
        ScrollBar.vertical: ScrollBar {
            id: scrollBar
            policy: ScrollBar.AsNeeded
            contentItem: Rectangle {
                implicitWidth: 6
                radius: 3
                color: root.mutedForeground
                visible: scrollBar.size > 0 && scrollBar.size < 1
                opacity: scrollBar.active ? 0.65 : 0.25
            }
        }

        TextEdit {
            id: edit
            readOnly: root.readOnly
            // Accessibility can query sibling geometry during construction. A zero-size
            // lookup enables TextEdit's expensive implicit-width measurement on every edit.
            visible: false
            Component.onCompleted: visible = true
            x: root.contentPadding
            y: root.topPadding
            width: Math.max(1, viewport.width - root.contentPadding * 2)
            height: Math.max(contentHeight, viewport.height - root.topPadding - 28)
            focus: true
            textFormat: TextEdit.PlainText
            wrapMode: TextEdit.Wrap
            selectByMouse: true
            persistentSelection: true
            color: root.foreground
            selectionColor: root.selectionColor
            selectedTextColor: root.selectedTextColor
            font.family: root.fontFamily
            font.pointSize: root.fontSize
            // Keep source characters separate: Geist Mono ligates ### into a single glyph.
            font.preferShaping: false
            Accessible.name: qsTr("Markdown editor")
            onSelectionStartChanged: {
                root._formattingDismissed = false;
                if (selectionStart !== selectionEnd)
                    Qt.callLater(formatting.refreshGeometry);
            }
            onSelectionEndChanged: {
                root._formattingDismissed = false;
                if (selectionStart !== selectionEnd)
                    Qt.callLater(formatting.refreshGeometry);
            }
            onWidthChanged: if (selectionStart !== selectionEnd)
                Qt.callLater(formatting.refreshGeometry)
            onFontChanged: if (selectionStart !== selectionEnd)
                Qt.callLater(formatting.refreshGeometry)
            onContentHeightChanged: if (selectionStart !== selectionEnd)
                Qt.callLater(formatting.refreshGeometry)
            onCursorRectangleChanged: {
                if (selectionStart !== selectionEnd)
                    Qt.callLater(formatting.refreshGeometry);
                if (activeFocus && !viewport.moving)
                    Qt.callLater(root.ensureCursorVisible);
            }
            Keys.priority: Keys.BeforeItem
            Keys.onPressed: event => {
                if (event.matches(StandardKey.Paste)) {
                    root.paste();
                    event.accepted = true;
                }
            }
            Keys.onEscapePressed: event => {
                event.accepted = formatting.visible;
                root._formattingDismissed = true;
            }
            Keys.onReturnPressed: event => {
                if (root.readOnly || inputMethodComposing || selectionStart !== selectionEnd || event.modifiers !== Qt.NoModifier) {
                    event.accepted = false;
                    return;
                }
                const position = root.document.continueLine(cursorPosition);
                event.accepted = position >= 0;
                if (event.accepted)
                    cursorPosition = position;
            }
            Text {
                visible: edit.length === 0 && !edit.inputMethodComposing
                text: root.placeholderText
                color: root.mutedForeground
                font: edit.font
            }
        }
    }
    MarkdownDropArea {
        id: dropArea
        objectName: "markdown-drop-area"
        anchors.fill: viewport
        z: 1
        clip: true
        enabled: !root.readOnly && !edit.inputMethodComposing
        document: root.document
        insertionPosition: edit.positionAt(dragPosition.x - edit.x, dragPosition.y + viewport.contentY - edit.y)
        onDropping: position => {
            edit.cursorPosition = position;
            edit.forceActiveFocus();
        }

        Rectangle {
            readonly property rect caret: edit.positionToRectangle(dropArea.insertionPosition)
            visible: dropArea.containsDrag
            x: edit.x + caret.x
            y: edit.y + caret.y - viewport.contentY
            width: 2
            height: caret.height
            color: root.foreground
        }

        Timer {
            interval: 16
            repeat: true
            running: dropArea.containsDrag
            onTriggered: {
                const margin = Math.min(48, viewport.height / 4);
                const y = dropArea.dragPosition.y;
                const speed = y < margin ? (y - margin) / 4 : y > viewport.height - margin ? (y - viewport.height + margin) / 4 : 0;
                viewport.contentY = Math.max(0, Math.min(viewport.contentHeight - viewport.height, viewport.contentY + speed));
            }
        }
    }
    PointHandler {
        id: selectionPointer
        parent: viewport
        target: null
        acceptedButtons: Qt.LeftButton
    }
    SelectionToolbar {
        id: formatting
        objectName: "markdown-selection-toolbar"
        z: 2
        foreground: root.foreground
        backgroundColor: root.toolbarBackground
        borderColor: root.toolbarBorder
        hoverColor: root.toolbarHover
        icons: root.formattingIcons

        property rect startRect: Qt.rect(0, 0, 0, 0)
        property rect endRect: Qt.rect(0, 0, 0, 0)
        function refreshGeometry() {
            // TextEdit finishes reflow after its geometry change notifications.
            if (hasSelection) {
                startRect = edit.positionToRectangle(edit.selectionStart);
                endRect = edit.positionToRectangle(edit.selectionEnd);
            }
        }
        readonly property bool hasSelection: edit.selectionStart !== edit.selectionEnd
        readonly property real selectionTop: edit.y + startRect.y - viewport.contentY
        readonly property real selectionBottom: edit.y + endRect.y + endRect.height - viewport.contentY
        readonly property bool above: selectionTop >= height + 8
        readonly property real anchorX: edit.x + startRect.x

        width: Math.min(implicitWidth, root.width - 12)
        height: implicitHeight
        x: Math.max(6, Math.min(root.width - width - 6, anchorX - width / 2))
        y: Math.max(6, Math.min(root.height - height - 6, above ? selectionTop - height - 8 : selectionTop + startRect.height + 8))
        visible: !root.readOnly && hasSelection && root.Window.active && !root._formattingDismissed && !edit.inputMethodComposing && !selectionPointer.active && !viewport.moving && !dropArea.containsDrag && !editMenu.visible && root.activeFocus && selectionTop < root.height && selectionBottom > 0
        onFormatRequested: format => root.applyFormat(format)
        onDismissed: {
            root._formattingDismissed = true;
            edit.forceActiveFocus();
        }
    }
    Shortcut {
        sequence: StandardKey.Bold
        enabled: !root.readOnly && root.activeFocus && !edit.inputMethodComposing
        onActivated: root.applyFormat(MarkdownEditorDocument.Bold)
    }
    Shortcut {
        sequence: StandardKey.Italic
        enabled: !root.readOnly && root.activeFocus && !edit.inputMethodComposing
        onActivated: root.applyFormat(MarkdownEditorDocument.Italic)
    }
    Menu {
        id: editMenu
        popupType: Popup.Native
        MenuItem {
            text: qsTr("Undo")
            enabled: !root.readOnly && edit.canUndo
            onTriggered: edit.undo()
        }
        MenuItem {
            text: qsTr("Redo")
            enabled: !root.readOnly && edit.canRedo
            onTriggered: edit.redo()
        }
        MenuSeparator {}
        MenuItem {
            text: qsTr("Cut")
            enabled: !root.readOnly && edit.selectedText.length > 0
            onTriggered: edit.cut()
        }
        MenuItem {
            text: qsTr("Copy")
            enabled: edit.selectedText.length > 0
            onTriggered: edit.copy()
        }
        MenuItem {
            text: qsTr("Paste")
            enabled: !root.readOnly && (edit.canPaste || root.canPasteAttachment)
            onTriggered: root.paste()
        }
        MenuItem {
            text: qsTr("Select All")
            onTriggered: edit.selectAll()
        }
    }
    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: editMenu.popup()
    }
}
