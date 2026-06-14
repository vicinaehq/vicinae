import QtQuick

// Click-to-edit text cell that builds its input lazily (cheap in dense lists).
Rectangle {
    id: root

    property string text: ""
    property string placeholder: ""
    signal committed(string value)

    property bool _editing: false

    implicitWidth: 120
    implicitHeight: 24
    radius: 4
    color: "transparent"
    border.color: Config.withAlpha(root._editing ? Theme.inputBorderFocus : Theme.inputBorder, Config.windowOpacity)
    border.width: root._editing || hover.hovered ? 1 : 0

    HoverHandler {
        id: hover
    }

    Text {
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6
        visible: !root._editing
        text: root.text !== "" ? root.text : root.placeholder
        color: root.text !== "" ? Theme.foreground : Theme.textPlaceholder
        font.pointSize: Theme.smallerFontSize
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.IBeamCursor
            onClicked: root._editing = true
        }
    }

    Loader {
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6
        active: root._editing

        sourceComponent: TextInput {
            text: root.text
            font.pointSize: Theme.smallerFontSize
            color: Theme.foreground
            horizontalAlignment: TextInput.AlignHCenter
            verticalAlignment: TextInput.AlignVCenter
            clip: true

            Component.onCompleted: {
                forceActiveFocus();
                selectAll();
            }
            onActiveFocusChanged: {
                if (!activeFocus && root._editing) {
                    root.committed(text);
                    root._editing = false;
                }
            }
            onAccepted: {
                root.committed(text);
                root._editing = false;
            }
        }
    }
}
