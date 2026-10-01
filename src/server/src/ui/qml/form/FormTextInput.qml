pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

FocusScope {
    id: root
    implicitHeight: 36
    Layout.fillWidth: true
    activeFocusOnTab: !readOnly

    property alias text: editor.text
    property alias textDocument: editor.textDocument
    property alias highlighter: editor.highlighter
    property alias cursorPosition: editor.cursorPosition
    property alias placeholder: editor.placeholder
    property alias readOnly: editor.readOnly
    property alias accessibleLabel: editor.accessibleLabel
    property alias completions: editor.completions
    property alias triggerChar: editor.triggerChar
    property bool hasError: false
    property bool filled: false
    // if set to true, pressing escape or enter/return will defocus the input field
    // we usually want that on in settings window but not in form commands
    property bool releaseFocusOnAccept: false
    readonly property bool editing: editor.editing

    signal textEdited
    signal accepted

    function forceActiveFocus() {
        editor.forceActiveFocus();
    }
    function selectAll() {
        editor.selectAll();
    }

    onActiveFocusChanged: {
        if (activeFocus && !readOnly)
            editor.forceActiveFocus();
    }

    FormInputFrame {
        anchors.fill: parent
        filled: root.filled
        hasError: root.hasError
        focused: editor.editing && !root.readOnly
        dimmed: root.readOnly
    }

    FormTextEditor {
        id: editor
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        height: implicitHeight
        opacity: root.readOnly ? 0.5 : 1.0

        onTextEdited: root.textEdited()
        onAccepted: {
            root.accepted();
            if (root.releaseFocusOnAccept)
                editor.releaseFocus();
        }
        onEscaped: {
            if (root.releaseFocusOnAccept)
                editor.releaseFocus();
        }
    }
}
