import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    implicitHeight: 36
    Layout.fillWidth: true
    activeFocusOnTab: true

    property alias text: innerInput.text
    property string placeholder: ""
    property bool readOnly: false
    property bool hasError: false
    readonly property bool editing: innerInput.editing

    // [{iconSource, title, value}]
    property var completions: []

    property string triggerChar: "{"

    signal textEdited
    signal accepted

    function forceActiveFocus() {
        innerInput.forceActiveFocus();
    }
    function selectAll() {
        innerInput.selectAll();
    }

    onActiveFocusChanged: {
        if (activeFocus)
            innerInput.forceActiveFocus();
    }

    FormTextInput {
        id: innerInput
        anchors.fill: parent
        placeholder: root.placeholder
        readOnly: root.readOnly
        hasError: root.hasError

        onTextEdited: {
            root.textEdited();
            completer.update(innerInput.text, innerInput.cursorPosition);
        }
        onAccepted: {
            if (completer.active)
                completer.accept();
            else
                root.accepted();
        }

        Keys.onUpPressed: event => {
            if (completer.active) {
                event.accepted = true;
                completer.moveUp();
            }
        }
        Keys.onDownPressed: event => {
            if (completer.active) {
                event.accepted = true;
                completer.moveDown();
            }
        }
        Keys.onEscapePressed: event => {
            if (completer.active) {
                event.accepted = true;
                completer.dismiss();
            }
        }
    }

    PlaceholderCompleter {
        id: completer
        anchors.fill: parent
        completions: root.completions
        triggerChar: root.triggerChar
        text: innerInput.text
        cursorPosition: innerInput.cursorPosition
        onCompletionAccepted: (newText, newCursorPos) => {
            innerInput.text = newText;
            innerInput.cursorPosition = newCursorPos;
            root.textEdited();
        }
    }
}
