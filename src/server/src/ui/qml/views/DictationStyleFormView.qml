pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root
    required property DictationStyleFormViewHost host

    FormView {
        id: formView
        anchors.fill: parent
        Component.onCompleted: Qt.callLater(formView.focusFirst)

        RowLayout {
            Layout.fillWidth: true
            spacing: 20

            ColumnLayout {
                id: leftColumn
                Layout.fillWidth: true
                Layout.preferredWidth: 3
                Layout.alignment: Qt.AlignTop
                spacing: 12

                FormField {
                    id: nameField
                    stacked: true
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
                    stacked: true
                    label: qsTr("Instructions")
                    error: root.host.instructionsError

                    FormTextArea {
                        id: instructionsInput
                        // fills what the form has left, the text scrolls inside when it needs more
                        readonly property int rows: Math.max(5, Math.floor((formView.usableHeight - formView.padding * 2 - nameField.height - leftColumn.spacing - instructionsField.stackedLabelHeight - _verticalPadding * 2) / _lineHeight))
                        minRows: rows
                        maxRows: rows
                        text: root.host.instructions
                        placeholder: qsTr("Fix grammar and remove filler words. Keep my wording.")
                        hasError: instructionsField.error !== ""
                        onTextEdited: root.host.instructions = text
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.preferredWidth: 2
                Layout.alignment: Qt.AlignTop
                spacing: 12

                FormField {
                    id: modelField
                    stacked: true
                    label: qsTr("Model")
                    error: root.host.modelError

                    SearchableDropdown {
                        items: root.host.modelItems
                        currentItem: root.host.currentModel
                        hasError: modelField.error !== ""
                        onActivated: item => root.host.selectModel(item)
                    }
                }

                FormField {
                    stacked: true
                    label: qsTr("Applications")
                    error: root.host.appsError
                    info: qsTr("Used when you finish dictating in one of these apps.")

                    FormAppSelector {
                        model: root.host.apps
                        appsModel: root.host.appSelectorModel.model
                        placeholder: qsTr("+ Add app…")
                        emptyText: ""
                        onChanged: apps => root.host.apps = apps
                    }
                }

                FormField {
                    stacked: true
                    info: qsTr("Used in apps that have no style of their own.")

                    FormCheckbox {
                        label: qsTr("Use as default")
                        checked: root.host.useAsDefault
                        onToggled: root.host.useAsDefault = checked
                    }
                }
            }
        }
    }
}
