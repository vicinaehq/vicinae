import QtQuick
import QtQuick.Layouts

Item {
    id: root
    implicitHeight: 36
    Layout.fillWidth: true
    activeFocusOnTab: true

    property alias text: input.text
    property alias cursorPosition: input.cursorPosition
    property string placeholder: ""
    property bool readOnly: false
    property bool hasError: false
    property alias echoMode: input.echoMode
    readonly property bool editing: input.activeFocus

    signal textEdited()
    signal accepted()

    function forceActiveFocus() { input.forceActiveFocus() }
    function selectAll() { input.selectAll() }


    onActiveFocusChanged: {
        if (activeFocus) input.forceActiveFocus()
    }

    Rectangle {
        id: border
        anchors.fill: parent
        radius: 8
        color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g,
                       Theme.secondaryBackground.b, Config.windowOpacity)
        border.color: root.hasError ? Theme.inputBorderError
                      : input.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder
        border.width: 1

        TextInput {
            id: input
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            verticalAlignment: TextInput.AlignVCenter
            font.pointSize: Theme.regularFontSize
            color: Theme.foreground
            selectionColor: Theme.textSelectionBg
            selectedTextColor: Theme.textSelectionFg
            readOnly: root.readOnly
            clip: true
            activeFocusOnTab: false

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                text: root.placeholder
                color: Theme.textPlaceholder
                font: input.font
                visible: !input.text && !input.preeditText
            }

            onTextEdited: root.textEdited()
            onAccepted: root.accepted()

            Keys.onPressed: (event) => {
                if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                    event.accepted = false
                    return
                }
                if (typeof launcher !== "undefined")
                    event.accepted = launcher.forwardKey(event.key, event.modifiers)
            }
        }
    }
}
