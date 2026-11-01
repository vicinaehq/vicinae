pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    required property DictationStyleFormViewHost host

    FormView {
        id: formView
        anchors.fill: parent
        Component.onCompleted: Qt.callLater(formView.focusFirst)

        FormField {
            id: nameField
            label: qsTr("Name")
            error: root.host.nameError

            FormTextInput {
                text: root.host.name
                placeholder: qsTr("Clean up")
                hasError: nameField.error !== ""
                onTextEdited: root.host.name = text
            }
        }

        FormField {
            id: instructionsField
            label: qsTr("Instructions")
            error: root.host.instructionsError
            info: qsTr("How the model should rewrite what you dictate.")
            topAlignLabel: true

            FormTextArea {
                text: root.host.instructions
                placeholder: qsTr("Fix grammar and remove filler words. Keep my wording.")
                hasError: instructionsField.error !== ""
                onTextEdited: root.host.instructions = text
            }
        }

        FormField {
            id: modelField
            label: qsTr("Model")
            error: root.host.modelError
            info: qsTr("The transcript is sent to this model. Fast models keep dictation snappy.")

            SearchableDropdown {
                items: root.host.modelItems
                currentItem: root.host.currentModel
                hasError: modelField.error !== ""
                onActivated: item => root.host.selectModel(item)
            }
        }

        FormSeparator {}

        FormField {
            label: qsTr("Applications")
            error: root.host.appsError
            info: qsTr("This style is used when you finish dictating in one of these apps. An app can only belong to one style.")
            topAlignLabel: true

            FormAppSelector {
                model: root.host.apps
                appsModel: root.host.appSelectorModel.model
                placeholder: qsTr("+ Add app…")
                emptyText: qsTr("No applications")
                onChanged: apps => root.host.apps = apps
            }
        }

        FormField {
            label: qsTr("Use as default")
            info: qsTr("Used in apps that have no style of their own.")

            FormCheckbox {
                checked: root.host.useAsDefault
                onToggled: root.host.useAsDefault = checked
            }
        }
    }
}
