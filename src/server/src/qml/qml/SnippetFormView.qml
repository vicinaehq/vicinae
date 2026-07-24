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
            label: qsTr("Title")
            error: root.host.nameError

            FormTextInput {
                text: root.host.name
                placeholder: qsTr("Euro symbol")
                hasError: titleField.error !== ""
                onTextEdited: root.host.name = text
            }
        }

        FormField {
            id: contentField
            label: qsTr("Content")
            error: root.host.contentError
            info: qsTr("You can use {dynamic placeholders} to make the content dynamic: <a href=\"https://docs.vicinae.com/snippets\">learn more</a>.")
            topAlignLabel: true

            FormCompletedTextArea {
                text: root.host.content
                placeholder: "€"
                completions: root.host.contentCompletions
                hasError: contentField.error !== ""
                onTextEdited: root.host.content = text
            }
        }

        FormSeparator {}

        FormField {
            id: keywordField
            label: qsTr("Keyword")
            error: root.host.keywordError
            info: root.host.serverRunning ? qsTr("Typing this keyword anywhere will result in it being replaced by the content of the snippet.") : qsTr("The snippet server is not running. Keyword expansion is unavailable. <a href=\"https://docs.vicinae.com/snippets\">Learn more</a>.")

            FormTextInput {
                text: root.host.keyword
                placeholder: ":!euro"
                hasError: keywordField.error !== ""
                readOnly: !root.host.serverRunning
                onTextEdited: root.host.keyword = text
            }
        }

        FormField {
            label: qsTr("Applications")
            info: qsTr("Restrict expansion to specific applications. By default, it works everywhere.")
            visible: root.host.serverRunning && root.host.keyword !== ""
            topAlignLabel: true

            FormAppSelector {
                model: root.host.apps
                sections: root.host.availableApps
                onChanged: apps => root.host.apps = apps
            }
        }

        FormField {
            label: qsTr("Expand as word")
            info: qsTr("If a keyword is typed, it will only be expanded after space or punctuation.")
            visible: root.host.serverRunning

            FormCheckbox {
                checked: root.host.expandAsWord
                onToggled: root.host.expandAsWord = checked
            }
        }
    }
}
