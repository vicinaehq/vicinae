pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

ViciModal {
    id: root

    property string title: ""
    property string description: ""
    property string placeholder: ""
    property int recommendedLength: 4000

    signal saved(string text)

    function openWith(text) {
        editor.text = text;
        open();
        editor.forceActiveFocus();
    }

    parent: Overlay.overlay
    width: Math.min(parent.width - 40, 560)
    padding: 24

    contentItem: ColumnLayout {
        spacing: 12

        Text {
            text: root.title
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize + 1
            font.bold: true
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        Text {
            visible: root.description !== ""
            text: root.description
            color: Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        FormTextArea {
            id: editor
            Layout.fillWidth: true
            minRows: 8
            maxRows: 16
            placeholder: root.placeholder
            hasError: editor.text.length > root.recommendedLength
        }

        Text {
            readonly property bool tooLong: editor.text.length > root.recommendedLength
            text: tooLong ? qsTr("%1 characters. Long text is sent with every message and costs more.").arg(editor.text.length) : qsTr("%n character(s)", "", editor.text.length)
            color: tooLong ? Theme.danger : Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: 4
            spacing: 10

            ViciButton {
                Layout.fillWidth: true
                implicitHeight: 32
                variant: "ghost"
                bordered: true
                text: qsTr("Cancel")
                activeFocusOnTab: true
                onClicked: root.close()
            }

            ViciButton {
                Layout.fillWidth: true
                implicitHeight: 32
                variant: "accent"
                text: qsTr("Save")
                activeFocusOnTab: true
                onClicked: {
                    root.saved(editor.text.trim());
                    root.close();
                }
            }
        }
    }
}
