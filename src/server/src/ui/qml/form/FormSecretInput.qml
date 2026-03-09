import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root
    Layout.fillWidth: true
    spacing: 4

    property alias text: field.text
    property alias placeholder: field.placeholder
    property alias hasError: field.hasError
    readonly property bool editing: field.editing
    property bool revealed: false

    signal accepted

    function forceActiveFocus() {
        field.forceActiveFocus();
    }

    FormTextInput {
        id: field
        Layout.fillWidth: true
        echoMode: root.revealed ? TextInput.Normal : TextInput.Password
        onAccepted: root.accepted()
    }

    Rectangle {
        Layout.preferredWidth: 36
        Layout.preferredHeight: 36
        radius: 8
        color: "transparent"
        border.color: revealHover.hovered ? Theme.inputBorderFocus : Theme.inputBorder
        border.width: 1

        ViciImage {
            anchors.centerIn: parent
            source: Img.builtin(root.revealed ? "eye-disabled" : "eye").withFillColor(Theme.textMuted)
            width: 16
            height: 16
        }

        HoverHandler {
            id: revealHover
        }
        TapHandler {
            onTapped: root.revealed = !root.revealed
        }
    }
}
