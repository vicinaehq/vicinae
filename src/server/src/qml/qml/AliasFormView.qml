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
            label: "Alias"
            error: root.host.aliasError
            info: "Additional words to index this item against"

            FormTextInput {
                text: root.host.alias
                onTextEdited: root.host.alias = text
                onAccepted: launcher.handleReturn()
            }
        }
    }
}
