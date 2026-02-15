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
            label: "Author"
            error: root.host.authorError
            info: 'If you plan on submitting your extension to the <a href="vicinae://extensions/vicinae/vicinae/store">Vicinae store</a>, this must exactly match your GitHub handle. Otherwise, you can set it to anything.'

            FormTextInput {
                text: root.host.author
                placeholder: "Username"
                onTextEdited: root.host.author = text
                onAccepted: launcher.handleReturn()
            }
        }

        FormSeparator {}

        FormField {
            label: "Extension Title"
            error: root.host.titleError

            FormTextInput {
                text: root.host.title
                placeholder: "My Extension"
                onTextEdited: root.host.title = text
                onAccepted: launcher.handleReturn()
            }
        }

        FormField {
            label: "Description"
            error: root.host.descriptionError

            FormTextArea {
                text: root.host.description
                placeholder: "An extension that does super cool things"
                onTextEdited: root.host.description = text
            }
        }

        FormField {
            label: "Location"
            error: root.host.locationError

            FormTextInput {
                text: root.host.location
                placeholder: "~/code/vicinae-extensions"
                onTextEdited: root.host.location = text
                onAccepted: launcher.handleReturn()
            }
        }

        FormSeparator {}

        FormField {
            label: "Command Title"
            error: root.host.commandTitleError

            FormTextInput {
                text: root.host.commandTitle
                placeholder: "My Wonderful Command"
                onTextEdited: root.host.commandTitle = text
                onAccepted: launcher.handleReturn()
            }
        }

        FormField {
            label: "Subtitle"
            error: root.host.commandSubtitleError

            FormTextInput {
                text: root.host.commandSubtitle
                placeholder: "An helpful subtitle"
                onTextEdited: root.host.commandSubtitle = text
                onAccepted: launcher.handleReturn()
            }
        }

        FormField {
            label: "Description"
            error: root.host.commandDescriptionError

            FormTextArea {
                text: root.host.commandDescription
                placeholder: "My command does this, and that..."
                onTextEdited: root.host.commandDescription = text
            }
        }

        FormField {
            label: "Template"

            SearchableDropdown {
                items: root.host.templateItems
                currentItem: root.host.selectedTemplate
                onActivated: (item) => root.host.selectTemplate(item)
            }
        }
    }
}
