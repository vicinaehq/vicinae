import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

ViciModal {
    id: root

    property string modelId: ""
    property string modelName: ""

    signal confirmed(string modelId)

    function openFor(id, name) {
        modelId = id;
        modelName = name;
        open();
    }

    parent: Overlay.overlay
    width: Math.min(parent.width - 40, 420)
    padding: 24

    contentItem: ColumnLayout {
        spacing: 16

        Text {
            text: qsTr("Delete %1?").arg(root.modelName)
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize + 1
            font.bold: true
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        Text {
            text: qsTr("The model file will be removed from disk. You can download it again at any time.")
            color: Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            ViciButton {
                Layout.fillWidth: true
                implicitHeight: 32
                variant: "ghost"
                bordered: true
                text: qsTr("Cancel")
                focus: true
                activeFocusOnTab: true
                onClicked: root.close()
            }

            ViciButton {
                Layout.fillWidth: true
                implicitHeight: 32
                variant: "ghost"
                bordered: true
                foreground: Theme.danger
                text: qsTr("Delete")
                activeFocusOnTab: true
                onClicked: {
                    root.confirmed(root.modelId);
                    root.close();
                }
            }
        }
    }
}
