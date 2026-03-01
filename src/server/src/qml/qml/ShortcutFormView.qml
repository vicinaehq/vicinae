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
            label: "Name"

            FormTextInput {
                text: root.host.name
                placeholder: "Shortcut Name"
                onTextEdited: root.host.name = text
                onAccepted: launcher.handleReturn()
            }
        }

        FormField {
            id: urlField
            label: "URL"
            error: root.host.linkError
            info: "The URL that will be opened by the specified app. You can make it dynamic by using placeholders such as {argument}."

            FormCompletedInput {
                id: linkInput
                text: root.host.link
                placeholder: "https://google.com/search?q={argument}"
                completions: root.host.linkCompletions
                hasError: urlField.error !== ""

                onTextEdited: root.host.link = text
                onAccepted: launcher.handleReturn()

                onEditingChanged: {
                    if (!editing) root.host.handleLinkBlurred()
                }
            }
        }

        FormSeparator {}

        FormField {
            id: appField
            label: "Open with"
            error: root.host.appError

            SearchableDropdown {
                items: root.host.appSelectorModel.items
                currentItem: root.host.selectedApp
                hasError: appField.error !== ""
                onActivated: (item) => root.host.selectApp(item)
            }
        }

        FormField {
            id: iconField
            label: "Icon"
            error: root.host.iconError

            SearchableDropdown {
                items: root.host.iconItems
                currentItem: root.host.selectedIcon
                hasError: iconField.error !== ""
                onActivated: (item) => root.host.selectIcon(item)
            }
        }
    }
}
