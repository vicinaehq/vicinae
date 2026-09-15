pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    required property VocabularyFormViewHost host

    FormView {
        id: formView
        anchors.fill: parent
        Component.onCompleted: Qt.callLater(formView.focusFirst)

        FormField {
            id: wordField
            label: qsTr("Word")
            error: root.host.wordError
            info: qsTr("A name, term or acronym you dictate often and want transcribed exactly this way.")

            FormTextInput {
                text: root.host.word
                placeholder: qsTr("Vicinae")
                hasError: wordField.error !== ""
                onTextEdited: root.host.word = text
            }
        }
    }
}
