import QtQuick
import QtQuick.Layouts

Item {
    id: root
    required property var host

    FormView {
        id: formView
        anchors.fill: parent
        Component.onCompleted: Qt.callLater(formView.focusFirst)

        FormField {
            id: titleField
            label: "Title"
            error: root.host.nameError

            FormTextInput {
                text: root.host.name
                placeholder: "Euro symbol"
                hasError: titleField.error !== ""
                onTextEdited: root.host.name = text
                onAccepted: launcher.handleReturn()
            }
        }

        FormField {
            id: contentField
            label: "Content"
            error: root.host.contentError
            info: "You can use {dynamic placeholders} to make the content dynamic. No autocomplete is available for now."

            FormTextArea {
                text: root.host.content
                placeholder: "€"
                hasError: contentField.error !== ""
                onTextEdited: root.host.content = text
            }
        }

        FormSeparator {}

        FormField {
            id: keywordField
            label: "Keyword"
            error: root.host.keywordError
            info: "Typing this keyword anywhere will result in it being replaced by the content of the snippet."

            FormTextInput {
                text: root.host.keyword
                placeholder: ":!euro"
                hasError: keywordField.error !== ""
                onTextEdited: root.host.keyword = text
                onAccepted: launcher.handleReturn()
            }
        }

        FormField {
            label: "Expand as word"
            info: "If a keyword is typed, it will only be expanded after space or return is pressed."

            FormCheckbox {
                checked: root.host.expandAsWord
                onToggled: root.host.expandAsWord = checked
            }
        }
    }
}
