pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

Item {
    id: root

    property alias text: edit.text
    property alias cursorPosition: edit.cursorPosition
    property alias font: edit.font
    property alias textDocument: edit.textDocument
    property string placeholder: ""
    property bool readOnly: false
    property bool multiline: false
    property string accessibleLabel: ""
    property DocumentHighlighter highlighter: null

    // [{iconSource, title, value, template?, cursorOffset?}]
    property var completions: []
    property string triggerChar: "{"

    readonly property bool editing: edit.activeFocus
    readonly property real contentHeight: edit.contentHeight
    readonly property PlaceholderCompleter _completer: completer.item as PlaceholderCompleter
    readonly property bool _completing: _completer?.active ?? false

    property bool _suppressTextChanged: false

    signal textEdited
    signal accepted
    signal escaped

    implicitHeight: edit.contentHeight

    function forceActiveFocus() {
        edit.forceActiveFocus();
    }
    function releaseFocus() {
        edit.focus = false;
    }
    function selectAll() {
        edit.selectAll();
    }

    function _accept() {
        if (root._completing)
            root._completer.accept();
        else if (root.multiline)
            edit.insert(edit.cursorPosition, "\n");
        else
            root.accepted();
    }

    Binding {
        target: root.highlighter
        property: "textDocument"
        value: edit.textDocument
        when: root.highlighter !== null
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: edit.width
        contentHeight: edit.height
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        interactive: root.multiline

        ViciWheelHandler {
            target: root.multiline ? flickable : null
            blockTargetWheel: false
        }

        ScrollBar.vertical: ViciScrollBar {
            policy: root.multiline && flickable.contentHeight > flickable.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
        }

        TextEdit {
            id: edit
            Accessible.name: root.accessibleLabel !== "" ? root.accessibleLabel : root.placeholder
            Accessible.readOnly: root.readOnly
            width: root.multiline ? flickable.width : Math.max(contentWidth, flickable.width)
            height: root.multiline ? Math.max(contentHeight, flickable.height) : contentHeight
            font.pointSize: Theme.regularFontSize
            color: Theme.foreground
            selectionColor: Theme.textSelectionBg
            selectedTextColor: Theme.textSelectionFg
            wrapMode: root.multiline ? TextEdit.Wrap : TextEdit.NoWrap
            selectByMouse: true
            cursorDelegate: ViciTextCursor {}
            readOnly: root.readOnly
            activeFocusOnTab: false

            Text {
                anchors.top: parent.top
                width: parent.width
                text: root.placeholder
                color: Theme.textPlaceholder
                font: edit.font
                wrapMode: root.multiline ? Text.Wrap : Text.NoWrap
                elide: root.multiline ? Text.ElideNone : Text.ElideRight
                visible: !edit.text && !edit.preeditText
            }

            onTextChanged: {
                if (root._suppressTextChanged)
                    return;
                if (!root.multiline && text.includes("\n")) {
                    TextDocumentEdit.replace(edit.textDocument, 0, edit.length, text.replace(/\n/g, " "));
                    return;
                }
                root.textEdited();
                root._completer?.update(edit.text, edit.cursorPosition);
            }

            onCursorRectangleChanged: {
                const rect = cursorRectangle;
                if (rect.x < flickable.contentX)
                    flickable.contentX = rect.x;
                else if (rect.x + rect.width > flickable.contentX + flickable.width)
                    flickable.contentX = rect.x + rect.width - flickable.width;
                if (rect.y < flickable.contentY)
                    flickable.contentY = rect.y;
                else if (rect.y + rect.height > flickable.contentY + flickable.height)
                    flickable.contentY = rect.y + rect.height - flickable.height;
            }

            Keys.priority: Keys.BeforeItem

            Keys.onTabPressed: event => {
                event.accepted = true;
                nextItemInFocusChain()?.forceActiveFocus(Qt.TabFocusReason);
            }
            Keys.onBacktabPressed: event => {
                event.accepted = true;
                nextItemInFocusChain(false)?.forceActiveFocus(Qt.BacktabFocusReason);
            }
            Keys.onReturnPressed: event => {
                event.accepted = true;
                root._accept();
            }
            Keys.onEnterPressed: event => {
                event.accepted = true;
                root._accept();
            }
            Keys.onUpPressed: event => {
                event.accepted = root._completing;
                if (root._completing)
                    root._completer.moveUp();
            }
            Keys.onDownPressed: event => {
                event.accepted = root._completing;
                if (root._completing)
                    root._completer.moveDown();
            }
            Keys.onEscapePressed: event => {
                if (root._completing) {
                    event.accepted = true;
                    root._completer.dismiss();
                } else {
                    root.escaped();
                }
            }
        }
    }

    Loader {
        id: completer
        anchors.fill: parent
        active: root.completions.length > 0

        sourceComponent: PlaceholderCompleter {
            completions: root.completions
            triggerChar: root.triggerChar
            text: edit.text
            cursorPosition: edit.cursorPosition
            onCompletionAccepted: (start, end, replacement, newCursorPos) => {
                root._suppressTextChanged = true;
                TextDocumentEdit.replace(edit.textDocument, start, end, replacement);
                edit.cursorPosition = newCursorPos;
                root._suppressTextChanged = false;
                root.textEdited();
            }
        }
    }
}
