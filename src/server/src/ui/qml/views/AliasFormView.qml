import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root
    required property AliasFormViewHost host

    FormView {
        id: formView
        anchors.fill: parent
        Component.onCompleted: Qt.callLater(formView.focusFirst)

        FormField {
            id: aliasField
            label: qsTr("Alias")
            error: root.host.aliasError
            info: qsTr("Additional words to index this item against")

            FormTextInput {
                text: root.host.alias
                hasError: aliasField.error !== ""
                onTextEdited: root.host.alias = text
            }
        }
    }
}
