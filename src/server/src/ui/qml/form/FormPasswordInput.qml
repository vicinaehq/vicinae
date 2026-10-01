pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

FocusScope {
    id: root
    implicitHeight: 36
    Layout.fillWidth: true
    activeFocusOnTab: !readOnly

    property alias text: input.text
    property string placeholder: ""
    property bool readOnly: false
    property bool hasError: false
    property bool filled: false
    property bool revealed: false
    property bool releaseFocusOnAccept: false
    property string accessibleLabel: ""
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

    FormInputFrame {
        anchors.fill: parent
        filled: root.filled
        hasError: root.hasError
        focused: input.activeFocus && !root.readOnly
        dimmed: root.readOnly
    }

    TextInput {
        id: input
        Accessible.name: root.accessibleLabel !== "" ? root.accessibleLabel : root.placeholder
        Accessible.readOnly: root.readOnly
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: revealBtn.width + 8
        opacity: root.readOnly ? 0.5 : 1.0
        verticalAlignment: TextInput.AlignVCenter
        font.pointSize: Theme.regularFontSize
        color: Theme.foreground
        selectionColor: Theme.textSelectionBg
        selectedTextColor: Theme.textSelectionFg
        echoMode: root.revealed ? TextInput.Normal : TextInput.Password
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

    ViciButton {
        id: revealBtn
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        width: 28
        height: 28
        radius: 6
        variant: "ghost"
        iconSource: Img.icon(root.revealed ? BuiltinIcon.EyeDisabled : BuiltinIcon.Eye).withFillColor(Theme.textMuted)
        accessibleName: root.revealed ? qsTr("Hide password") : qsTr("Show password")
        onClicked: root.revealed = !root.revealed
    }
}
