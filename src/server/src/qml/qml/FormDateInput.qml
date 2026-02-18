import QtQuick
import QtQuick.Layouts

Item {
    id: root
    implicitHeight: 36
    Layout.fillWidth: true
    activeFocusOnTab: true

    property alias text: input.text
    property bool includeTime: false
    property string minDate: ""
    property string maxDate: ""

    signal textEdited()

    function forceActiveFocus() { input.forceActiveFocus() }

    readonly property string _format: includeTime ? "YYYY-MM-DD HH:mm" : "YYYY-MM-DD"

    onActiveFocusChanged: {
        if (activeFocus) input.forceActiveFocus()
    }

    Rectangle {
        id: border
        anchors.fill: parent
        radius: 8
        color: Theme.secondaryBackground
        border.color: input.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder
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
            clip: true
            activeFocusOnTab: false

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                text: root._format
                color: Theme.textPlaceholder
                font: input.font
                visible: !input.text && !input.preeditText
            }

            onTextEdited: root.textEdited()

            onActiveFocusChanged: {
                if (!activeFocus) _validate()
            }

            Keys.onReturnPressed: (event) => {
                if (event.modifiers !== Qt.NoModifier) {
                    event.accepted = launcher.forwardKey(event.key, event.modifiers)
                } else {
                    event.accepted = false
                }
            }
            Keys.onPressed: (event) => {
                if (event.modifiers !== Qt.NoModifier && event.modifiers !== Qt.ShiftModifier
                    && event.key !== Qt.Key_Shift && event.key !== Qt.Key_Control
                    && event.key !== Qt.Key_Alt && event.key !== Qt.Key_Meta) {
                    event.accepted = launcher.forwardKey(event.key, event.modifiers)
                } else {
                    event.accepted = false
                }
            }
        }
    }

    function _validate() {
        if (input.text === "") return
        var date = new Date(input.text)
        if (isNaN(date.getTime())) return
        // Format to ISO
        if (root.includeTime) {
            var pad = (n) => n < 10 ? "0" + n : "" + n
            input.text = date.getFullYear() + "-" + pad(date.getMonth() + 1) + "-" + pad(date.getDate())
                + " " + pad(date.getHours()) + ":" + pad(date.getMinutes())
        } else {
            input.text = date.toISOString().substring(0, 10)
        }
        root.textEdited()
    }
}
