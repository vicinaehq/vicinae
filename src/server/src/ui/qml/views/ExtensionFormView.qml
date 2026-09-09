pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root
    required property ExtensionViewHost host
    required property ExtensionFormModel formModel

    property bool _autoFocusDone: false

    function restoreFocus() {
        _autoFocusDone = false;
        Qt.callLater(_tryAutoFocus);
    }

    Connections {
        target: root.formModel
        function onAutoFocusRequested(index) {
            root._autoFocusDone = false;
            Qt.callLater(root._tryAutoFocus);
        }
    }

    function _tryAutoFocus() {
        if (_autoFocusDone)
            return;
        for (var i = 0; i < repeater.count; i++) {
            var item = repeater.itemAt(i);
            if (item && item.autoFocus) {
                _autoFocusDone = true;
                Qt.callLater(function () {
                    item.focusField();
                });
                return;
            }
        }
        for (var j = 0; j < repeater.count; j++) {
            var item2 = repeater.itemAt(j);
            if (item2 && item2.isField) {
                _autoFocusDone = true;
                Qt.callLater(function () {
                    item2.focusField();
                });
                return;
            }
        }
    }

    FormView {
        id: formView
        anchors.fill: parent

        Repeater {
            id: repeater
            model: root.formModel

            delegate: FieldHost {}
        }
    }

    component FieldHost: Loader {
        id: fieldLoader
        Layout.fillWidth: true

        required property int index
        required property string type
        required property string fieldId
        required property string label
        required property string error
        required property string info
        required property string placeholder
        required property var value
        required property bool autoFocus
        required property var fieldData
        required property CompletionModel dropdownModel
        required property var currentDropdownItem

        readonly property bool isField: type !== "separator" && type !== "description"

        function focusField() {
            (item as FormField)?.focusField();
        }

        sourceComponent: {
            switch (type) {
            case "text":
                return textFieldComp;
            case "password":
                return passwordFieldComp;
            case "textarea":
                return textareaFieldComp;
            case "checkbox":
                return checkboxFieldComp;
            case "dropdown":
                return dropdownFieldComp;
            case "filepicker":
                return filepickerFieldComp;
            case "datepicker":
                return datepickerFieldComp;
            case "description":
                return descriptionFieldComp;
            case "separator":
                return separatorFieldComp;
            default:
                return null;
            }
        }
    }

    Component {
        id: textFieldComp
        FormField {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            error: field.host.error
            info: field.host.info

            focusTarget: textInput

            FormTextInput {
                id: textInput
                text: field.host.value != null ? String(field.host.value) : ""
                placeholder: field.host.placeholder
                hasError: field.error !== ""
                onTextEdited: root.formModel.setFieldValue(field.host.index, text)
                onActiveFocusChanged: {
                    if (activeFocus)
                        root.formModel.fieldFocused(field.host.index);
                    else
                        root.formModel.fieldBlurred(field.host.index);
                }
            }
        }
    }

    Component {
        id: passwordFieldComp
        FormField {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            error: field.host.error
            info: field.host.info

            focusTarget: passwordInput

            FormTextInput {
                id: passwordInput
                text: field.host.value != null ? String(field.host.value) : ""
                placeholder: field.host.placeholder
                hasError: field.error !== ""
                echoMode: TextInput.Password
                onTextEdited: root.formModel.setFieldValue(field.host.index, text)
                onActiveFocusChanged: {
                    if (activeFocus)
                        root.formModel.fieldFocused(field.host.index);
                    else
                        root.formModel.fieldBlurred(field.host.index);
                }
            }
        }
    }

    Component {
        id: textareaFieldComp
        FormField {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            error: field.host.error
            info: field.host.info
            topAlignLabel: true

            focusTarget: textArea

            FormTextArea {
                id: textArea
                text: field.host.value != null ? String(field.host.value) : ""
                placeholder: field.host.placeholder
                hasError: field.error !== ""
                onTextEdited: root.formModel.setFieldValue(field.host.index, text)
                // FormTextArea doesn't expose activeFocusChanged directly on the TextEdit,
                // so we track focus on the wrapper
                onActiveFocusChanged: {
                    if (activeFocus)
                        root.formModel.fieldFocused(field.host.index);
                    else
                        root.formModel.fieldBlurred(field.host.index);
                }
            }
        }
    }

    Component {
        id: checkboxFieldComp
        FormField {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            error: field.host.error
            info: field.host.info

            focusTarget: checkbox

            FormCheckbox {
                id: checkbox
                checked: field.host.value === true
                hasError: field.error !== ""
                label: field.host.fieldData && field.host.fieldData.label ? field.host.fieldData.label : ""
                onToggled: root.formModel.setFieldValue(field.host.index, checked)
                onActiveFocusChanged: {
                    if (activeFocus)
                        root.formModel.fieldFocused(field.host.index);
                    else
                        root.formModel.fieldBlurred(field.host.index);
                }
            }
        }
    }

    Component {
        id: dropdownFieldComp
        FormField {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            error: field.host.error
            info: field.host.info

            focusTarget: dropdown

            readonly property var _fd: field.host.fieldData || ({})

            SearchableDropdown {
                id: dropdown
                model: field.host.dropdownModel
                hasError: field.error !== ""
                currentItem: field.host.currentDropdownItem
                placeholder: field._fd.placeholder || field.host.placeholder || ""
                onActivated: item => {
                    root.formModel.setFieldValue(field.host.index, item.id);
                }
                onActiveFocusChanged: {
                    if (activeFocus)
                        root.formModel.fieldFocused(field.host.index);
                    else
                        root.formModel.fieldBlurred(field.host.index);
                }
            }
        }
    }

    Component {
        id: filepickerFieldComp
        FormField {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            error: field.host.error
            info: field.host.info

            focusTarget: filePicker

            readonly property var _fd: field.host.fieldData || ({})
            topAlignLabel: filePicker.multiple

            FormFilePicker {
                id: filePicker
                hasError: field.error !== ""
                multiple: field._fd.multiple || false
                canChooseFiles: field._fd.canChooseFiles !== undefined ? field._fd.canChooseFiles : true
                canChooseDirectories: field._fd.canChooseDirectories || false

                selectedPaths: {
                    const v = field.host.value;
                    if (!v || !v.length)
                        return [];
                    return Array.from(v);
                }
                onPathsChanged: paths => {
                    root.formModel.setFilePaths(field.host.index, paths);
                }
            }
        }
    }

    Component {
        id: datepickerFieldComp
        FormField {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            error: field.host.error
            info: field.host.info

            focusTarget: dateInput

            readonly property var _fd: field.host.fieldData || ({})

            FormDateInput {
                id: dateInput
                text: field.host.value != null ? String(field.host.value) : ""
                hasError: field.error !== ""
                includeTime: field._fd.includeTime || false
                minDate: field._fd.min || ""
                maxDate: field._fd.max || ""
                onTextEdited: root.formModel.setFieldValue(field.host.index, text)
                onActiveFocusChanged: {
                    if (activeFocus)
                        root.formModel.fieldFocused(field.host.index);
                    else
                        root.formModel.fieldBlurred(field.host.index);
                }
            }
        }
    }

    Component {
        id: descriptionFieldComp
        FormField {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            error: ""
            info: ""

            readonly property var _fd: field.host.fieldData || ({})

            Text {
                Layout.fillWidth: true
                text: field._fd.text || ""
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                wrapMode: Text.Wrap
            }
        }
    }

    Component {
        id: separatorFieldComp
        FormSeparator {}
    }

    Component.onCompleted: Qt.callLater(_tryAutoFocus)
}
