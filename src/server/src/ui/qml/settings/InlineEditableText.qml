pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

// Build the input only while editing so this stays cheap in dense lists.
Rectangle {
    id: root

    property string text: ""
    property string placeholder: ""
    property string accessibleName: ""
    property alias font: label.font
    property color foreground: text !== "" ? Theme.foreground : Theme.textPlaceholder
    property int horizontalAlignment: Text.AlignHCenter
    property real horizontalPadding: 6
    property bool readOnly: false
    property bool clickToEdit: true
    readonly property bool editing: _editing
    signal committed(string value)

    property bool _editing: false
    property string _originalText: ""

    function startEditing() {
        if (readOnly || _editing)
            return;
        _originalText = text;
        _editing = true;
    }

    function finishEditing() {
        const editor = input.item as TextInput;
        if (editor)
            commit(editor.text);
    }

    function cancelEditing() {
        _editing = false;
    }

    function commit(value) {
        if (!_editing)
            return;
        _editing = false;
        if (value !== _originalText)
            committed(value);
    }

    implicitWidth: Math.max(120, Math.ceil(label.implicitWidth) + 2 * horizontalPadding)
    implicitHeight: 24
    activeFocusOnTab: !readOnly && clickToEdit
    radius: 4
    color: "transparent"
    border.color: Config.withAlpha(root._editing ? Theme.inputBorderFocus : Theme.inputBorder, Config.surfaceOpacity)
    border.width: root._editing || (!readOnly && clickToEdit && (hover.hovered || activeFocus)) ? 1 : 0

    Accessible.role: Accessible.Button
    Accessible.name: accessibleName || text || placeholder
    Accessible.ignored: _editing || readOnly || !clickToEdit
    Accessible.onPressAction: startEditing()
    Keys.onReturnPressed: startEditing()
    Keys.onSpacePressed: startEditing()

    HoverHandler {
        id: hover
        enabled: !root.readOnly && root.clickToEdit
    }

    Text {
        id: label
        anchors.fill: parent
        anchors.leftMargin: root.horizontalPadding
        anchors.rightMargin: root.horizontalPadding
        visible: !root._editing
        text: root.text !== "" ? root.text : root.placeholder
        textFormat: Text.PlainText
        color: root.foreground
        font.family: Theme.fontFamily
        font.pointSize: Theme.smallerFontSize
        horizontalAlignment: root.horizontalAlignment
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight

        MouseArea {
            anchors.fill: parent
            enabled: !root.readOnly && root.clickToEdit
            cursorShape: Qt.IBeamCursor
            onClicked: root.startEditing()
        }
    }

    Loader {
        id: input
        anchors.fill: parent
        anchors.leftMargin: root.horizontalPadding
        anchors.rightMargin: root.horizontalPadding
        active: root._editing

        sourceComponent: TextInput {
            text: root._originalText
            font: root.font
            color: Theme.foreground
            horizontalAlignment: root.horizontalAlignment
            verticalAlignment: TextInput.AlignVCenter
            clip: true
            selectByMouse: true
            Accessible.name: root.accessibleName || root.placeholder
            selectionColor: Theme.textSelectionBg
            selectedTextColor: Theme.textSelectionFg

            Keys.onEscapePressed: root.cancelEditing()

            Component.onCompleted: {
                forceActiveFocus();
                selectAll();
            }
            onActiveFocusChanged: {
                if (!activeFocus)
                    root.commit(text);
            }
            onAccepted: root.commit(text)
        }
    }
}
