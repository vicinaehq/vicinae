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

    function focusEditor() {
        edit.forceActiveFocus();
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
            Accessible.name: qsTr("Markdown editor")
            onCursorRectangleChanged: {
                if (activeFocus && !viewport.moving)
                    Qt.callLater(root.ensureCursorVisible);
            }
            Keys.priority: Keys.BeforeItem
            Keys.onReturnPressed: event => {
                if (inputMethodComposing || selectionStart !== selectionEnd || event.modifiers !== Qt.NoModifier) {
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
    Shortcut {
        sequence: StandardKey.Bold
        enabled: root.activeFocus && !edit.inputMethodComposing
        onActivated: root.document.wrap(edit.selectionStart, edit.selectionEnd, "**")
    }
    Shortcut {
        sequence: StandardKey.Italic
        enabled: root.activeFocus && !edit.inputMethodComposing
        onActivated: root.document.wrap(edit.selectionStart, edit.selectionEnd, "*")
    }
    Menu {
        id: editMenu
        popupType: Popup.Native
        MenuItem {
            text: qsTr("Undo")
            enabled: edit.canUndo
            onTriggered: edit.undo()
        }
        MenuItem {
            text: qsTr("Redo")
            enabled: edit.canRedo
            onTriggered: edit.redo()
        }
        MenuSeparator {}
        MenuItem {
            text: qsTr("Cut")
            enabled: edit.selectedText.length > 0
            onTriggered: edit.cut()
        }
        MenuItem {
            text: qsTr("Copy")
            enabled: edit.selectedText.length > 0
            onTriggered: edit.copy()
        }
        MenuItem {
            text: qsTr("Paste")
            enabled: edit.canPaste
            onTriggered: edit.paste()
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
