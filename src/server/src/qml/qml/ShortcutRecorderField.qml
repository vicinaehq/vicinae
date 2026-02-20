import QtQuick
import QtQuick.Layouts

Item {
    id: root
    implicitHeight: 36

    property bool recording: false
    property string validationError: ""

    signal shortcutCaptured(int key, int modifiers)
    property var validateShortcut: null

    Rectangle {
        anchors.fill: parent
        radius: 8
        color: Theme.secondaryBackground
        border.color: root.recording ? Theme.accent
                      : root.activeFocus ? Theme.inputBorderFocus
                      : Theme.inputBorder
        border.width: root.recording ? 2 : 1

        Text {
            anchors.centerIn: parent
            text: root.recording ? "Press a shortcut..." : "Click to record"
            color: root.recording ? Theme.accent : Theme.textMuted
            font.pointSize: Theme.regularFontSize
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                root.recording = true
                root.validationError = ""
                root.forceActiveFocus()
            }
        }
    }

    Text {
        anchors.top: parent.bottom
        anchors.topMargin: 4
        text: root.validationError
        color: Theme.danger
        font.pointSize: Theme.smallerFontSize
        visible: root.validationError !== ""
    }

    Keys.onPressed: (event) => {
        if (!root.recording) return

        // Ignore bare modifier keys
        if (event.key === Qt.Key_Shift || event.key === Qt.Key_Control
            || event.key === Qt.Key_Alt || event.key === Qt.Key_Meta) {
            return
        }

        event.accepted = true

        if (event.key === Qt.Key_Escape) {
            root.recording = false
            root.validationError = ""
            return
        }

        if (root.validateShortcut) {
            var error = root.validateShortcut(event.key, event.modifiers)
            if (error !== "") {
                root.validationError = error
                return
            }
        }

        root.validationError = ""
        root.recording = false
        root.shortcutCaptured(event.key, event.modifiers)
    }

    onActiveFocusChanged: {
        if (!activeFocus) {
            root.recording = false
            root.validationError = ""
        }
    }
}
