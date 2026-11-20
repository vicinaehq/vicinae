pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root
    required property AiCommandFormViewHost host

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
                        placeholder: qsTr("Fix Spelling and Grammar")
                        hasError: nameField.error !== ""
                        onTextEdited: root.host.name = text
                    }
                }

                FormField {
                    id: promptField
                    stacked: true
                    label: qsTr("Prompt")
                    error: root.host.promptError
                    info: qsTr("Type { to insert the selected text, the clipboard or an argument. When pasted, the response replaces the selection if there is one.")

                    FormTextArea {
                        // fills what the form has left, the text scrolls inside when it needs more
                        readonly property int rows: Math.max(5, Math.floor((formView.usableHeight - formView.padding * 2 - nameField.height - leftColumn.spacing - promptField.stackedChromeHeight - _verticalPadding * 2) / _lineHeight))
                        minRows: rows
                        maxRows: rows
                        text: root.host.prompt
                        placeholder: qsTr("Fix the spelling and grammar of {selection}. Keep my wording and tone.")
                        completions: root.host.promptCompletions
                        highlighter: PlaceholderHighlight {}
                        hasError: promptField.error !== ""
                        onTextEdited: root.host.prompt = text
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
                    label: qsTr("Output")

                    SearchableDropdown {
                        items: root.host.outputItems
                        currentItem: root.host.currentOutput
                        onActivated: item => root.host.selectOutput(item)
                    }
                }

                FormField {
                    stacked: true
                    label: qsTr("Icon")

                    SearchableDropdown {
                        model: root.host.iconModel
                        currentItem: root.host.selectedIcon
                        onActivated: item => root.host.selectIcon(item)
                    }
                }
            }
        }
    }
}
