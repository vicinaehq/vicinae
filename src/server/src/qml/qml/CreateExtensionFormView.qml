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
            id: authorField
            label: qsTr("Author")
            error: root.host.authorError
            info: qsTr('If you plan on submitting your extension to the <a href="vicinae://launch/core/store">Vicinae store</a>, this must exactly match your GitHub handle. Otherwise, you can set it to anything.')

            FormTextInput {
                text: root.host.author
                placeholder: qsTr("Username")
                hasError: authorField.error !== ""
                onTextEdited: root.host.author = text
            }
        }

        FormSeparator {}

        FormField {
            id: extTitleField
            label: qsTr("Extension Title")
            error: root.host.titleError

            FormTextInput {
                text: root.host.title
                placeholder: qsTr("My Extension")
                hasError: extTitleField.error !== ""
                onTextEdited: root.host.title = text
            }
        }

        FormField {
            id: descriptionField
            label: qsTr("Description")
            error: root.host.descriptionError
            topAlignLabel: true

            FormTextArea {
                text: root.host.description
                placeholder: qsTr("An extension that does super cool things")
                hasError: descriptionField.error !== ""
                onTextEdited: root.host.description = text
            }
        }

        FormField {
            id: locationField
            label: qsTr("Location")
            error: root.host.locationError

            FormTextInput {
                text: root.host.location
                placeholder: "~/code/vicinae-extensions"
                hasError: locationField.error !== ""
                onTextEdited: root.host.location = text
            }
        }

        FormSeparator {}

        FormField {
            id: cmdTitleField
            label: qsTr("Command Title")
            error: root.host.commandTitleError

            FormTextInput {
                text: root.host.commandTitle
                placeholder: qsTr("My Wonderful Command")
                hasError: cmdTitleField.error !== ""
                onTextEdited: root.host.commandTitle = text
            }
        }

        FormField {
            id: cmdDescField
            label: qsTr("Description")
            error: root.host.commandDescriptionError
            topAlignLabel: true

            FormTextArea {
                text: root.host.commandDescription
                placeholder: qsTr("My command does this, and that...")
                hasError: cmdDescField.error !== ""
                onTextEdited: root.host.commandDescription = text
            }
        }

        FormField {
            label: qsTr("Template")

            SearchableDropdown {
                items: root.host.templateItems
                currentItem: root.host.selectedTemplate
                onActivated: item => root.host.selectTemplate(item)
            }
        }
    }
}
