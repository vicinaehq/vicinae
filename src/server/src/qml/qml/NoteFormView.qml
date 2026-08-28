import QtQuick

Item {
    id: root
    required property var host

    FormView {
        id: formView
        anchors.fill: parent
        Component.onCompleted: Qt.callLater(formView.focusFirst)

        FormField {
            id: titleField
            label: qsTr("Title")
            error: root.host.titleError

            FormTextInput {
                text: root.host.title
                placeholder: qsTr("Meeting notes")
                hasError: titleField.error !== ""
                onTextEdited: root.host.title = text
            }
        }

        FormField {
            label: qsTr("Body")
            info: qsTr("Markdown is rendered when previewing the note.")
            topAlignLabel: true

            FormTextArea {
                text: root.host.body
                placeholder: qsTr("## Agenda")
                minRows: 8
                onTextEdited: root.host.body = text
            }
        }
    }
}
