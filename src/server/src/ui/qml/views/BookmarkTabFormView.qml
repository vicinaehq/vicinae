pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    required property BookmarkTabFormViewHost host

    FormView {
        id: formView
        anchors.fill: parent
        Component.onCompleted: Qt.callLater(formView.focusFirst)

        FormField {
            label: qsTr("Title")

            FormTextInput {
                text: root.host.title
                onTextEdited: root.host.title = text
            }
        }

        FormField {
            id: urlField
            label: qsTr("URL")
            error: root.host.urlError

            FormTextInput {
                text: root.host.url
                hasError: urlField.error !== ""
                onTextEdited: root.host.url = text
            }
        }

        FormField {
            label: qsTr("Folder")

            SearchableDropdown {
                model: root.host.folderModel
                currentItem: root.host.selectedFolder
                onActivated: item => root.host.selectFolder(item)
            }
        }
    }
}
