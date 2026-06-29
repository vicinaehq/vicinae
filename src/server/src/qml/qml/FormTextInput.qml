import QtQuick
import QtQuick.Layouts

Item {
    id: root
    implicitHeight: 36
    Layout.fillWidth: true
    activeFocusOnTab: !readOnly

    property alias text: input.text
    property alias cursorPosition: input.cursorPosition
    property string placeholder: ""
    property bool readOnly: false
    property bool hasError: false
    property bool filled: false
    // if set to true, pressing escape or enter/return will defocus the input field
    // we usually want that on in settings window but not in form commands
    property bool releaseFocusOnAccept: false
    property alias echoMode: input.echoMode
    readonly property bool editing: input.activeFocus

    signal textEdited
    signal accepted

    function forceActiveFocus() {
        input.forceActiveFocus();
    }
    function selectAll() {
        input.selectAll();
    }

    onActiveFocusChanged: {
        if (activeFocus && !readOnly)
            input.forceActiveFocus();
    }

    FormInputBackground {
        anchors.fill: parent
        radius: 8
        filled: root.filled
        opacity: root.readOnly ? 0.5 : 1.0
    }

    Rectangle {
        anchors.fill: parent
        radius: 8
        color: "transparent"
        border.color: Config.withAlpha(root.hasError ? Theme.inputBorderError : input.activeFocus && !root.readOnly ? Theme.inputBorderFocus : Theme.inputBorder, Config.windowOpacity)
        border.width: 1
        opacity: root.readOnly ? 0.5 : 1.0
    }

    TextInput {
        id: input
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        opacity: root.readOnly ? 0.5 : 1.0
        verticalAlignment: TextInput.AlignVCenter
        font.pointSize: Theme.regularFontSize
        color: Theme.foreground
        selectionColor: Theme.textSelectionBg
        selectedTextColor: Theme.textSelectionFg
        readOnly: root.readOnly
        activeFocusOnTab: !root.readOnly
        clip: true

        Text {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            text: root.placeholder
            color: Theme.textPlaceholder
            font: input.font
            visible: !input.text && !input.preeditText
        }

        onTextEdited: root.textEdited()

        Keys.onEscapePressed: ev => {
            if (root.releaseFocusOnAccept) {
                input.focus = false;
                ev.accepted = true;
            }
        }

        onAccepted: {
            root.accepted();
            if (root.releaseFocusOnAccept)
                input.focus = false;
        }
    }
}
