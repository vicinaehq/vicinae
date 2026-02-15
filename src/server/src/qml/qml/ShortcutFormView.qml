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
            label: "URL"
            error: root.host.linkError
            info: "The URL that will be opened by the specified app. You can make it dynamic by using placeholders such as {argument}."

            FormCompletedInput {
                id: linkInput
                text: root.host.link
                placeholder: "https://google.com/search?q={argument}"
                completions: root.host.linkCompletions

                onTextEdited: root.host.link = text
                onCompletionActivated: (item) => {
                    var result = root.host.insertCompletion(text, cursorPosition, item)
                    root.host.link = result
                    text = result
                }
                onAccepted: launcher.handleReturn()

                onActiveFocusChanged: {
                    if (!activeFocus) root.host.handleLinkBlurred()
                }
            }
        }

        FormSeparator {}

        FormField {
            label: "Open with"
            error: root.host.appError

            FormSelectorDropdown {
                items: root.host.appSelectorModel.items
                currentItem: root.host.selectedApp
                onActivated: (item) => root.host.selectApp(item)
            }
        }

        FormField {
            label: "Icon"
            error: root.host.iconError

            FormSelectorDropdown {
                items: root.host.iconItems
                currentItem: root.host.selectedIcon
                onActivated: (item) => root.host.selectIcon(item)
            }
        }
    }
}
