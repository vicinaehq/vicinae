import QtQuick
import QtQuick.Layouts

Item {
    id: root
    required property var host

    FormView {
        id: formView
        anchors.fill: parent
        Component.onCompleted: {
            Qt.callLater(() => {
                formView.focusFirst()
                textArea.selectAll()
            })
        }

        FormField {
            label: "Keywords"
            info: root.host.infoText

            FormTextArea {
                id: textArea
                text: root.host.keywords
                onTextEdited: root.host.keywords = text
            }
        }
    }
}
