import QtQuick
import QtQuick.Layouts

Item {
    id: root
    required property var host
    required property var formModel

    property bool _autoFocusDone: false

    Connections {
        target: root.formModel
        function onAutoFocusRequested(index) {
            root._autoFocusDone = false
            Qt.callLater(root._tryAutoFocus)
        }
    }

    function _tryAutoFocus() {
        if (_autoFocusDone) return
        for (var i = 0; i < repeater.count; i++) {
            var item = repeater.itemAt(i)
            if (item && item.autoFocus) {
                _autoFocusDone = true
                Qt.callLater(function() { item.focusField() })
                return
            }
        }
        for (var j = 0; j < repeater.count; j++) {
            var item2 = repeater.itemAt(j)
            if (item2 && item2.isField) {
                _autoFocusDone = true
                Qt.callLater(function() { item2.focusField() })
                return
            }
        }
    }

    FormView {
        id: formView
        anchors.fill: parent

            Repeater {
                id: repeater
                model: root.formModel

                delegate: Loader {
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

                    readonly property bool isField: type !== "separator" && type !== "description"

                    function focusField() {
                        if (item && typeof item.focusField === "function")
                            item.focusField()
                    }

                    sourceComponent: {
                        switch (type) {
                        case "text": return textFieldComp
                        case "password": return passwordFieldComp
                        case "textarea": return textareaFieldComp
                        case "checkbox": return checkboxFieldComp
                        case "dropdown": return dropdownFieldComp
                        case "filepicker": return filepickerFieldComp
                        case "datepicker": return datepickerFieldComp
                        case "description": return descriptionFieldComp
                        case "separator": return separatorFieldComp
                        default: return null
                        }
                    }
                }
            }
        }

    Component {
        id: textFieldComp
        FormField {
            id: field
            label: parent.label
            error: parent.error
            info: parent.info

            function focusField() { textInput.forceActiveFocus() }

            FormTextInput {
                id: textInput
                text: field.parent.value != null ? String(field.parent.value) : ""
                placeholder: field.parent.placeholder
                hasError: field.error !== ""
                onTextEdited: root.formModel.setFieldValue(field.parent.index, text)
                onActiveFocusChanged: {
                    if (activeFocus) root.formModel.fieldFocused(field.parent.index)
                    else root.formModel.fieldBlurred(field.parent.index)
                }
            }
        }
    }

    Component {
        id: passwordFieldComp
        FormField {
            id: field
            label: parent.label
            error: parent.error
            info: parent.info

            function focusField() { passwordInput.forceActiveFocus() }

            FormTextInput {
                id: passwordInput
                text: field.parent.value != null ? String(field.parent.value) : ""
                placeholder: field.parent.placeholder
                hasError: field.error !== ""
                echoMode: TextInput.Password
                onTextEdited: root.formModel.setFieldValue(field.parent.index, text)
                onActiveFocusChanged: {
                    if (activeFocus) root.formModel.fieldFocused(field.parent.index)
                    else root.formModel.fieldBlurred(field.parent.index)
                }
            }
        }
    }

    Component {
        id: textareaFieldComp
        FormField {
            id: field
            label: parent.label
            error: parent.error
            info: parent.info

            function focusField() { textArea.forceActiveFocus() }

            FormTextArea {
                id: textArea
                text: field.parent.value != null ? String(field.parent.value) : ""
                placeholder: field.parent.placeholder
                hasError: field.error !== ""
                onTextEdited: root.formModel.setFieldValue(field.parent.index, text)
                // FormTextArea doesn't expose activeFocusChanged directly on the TextEdit,
                // so we track focus on the wrapper
                onActiveFocusChanged: {
                    if (activeFocus) root.formModel.fieldFocused(field.parent.index)
                    else root.formModel.fieldBlurred(field.parent.index)
                }
            }
        }
    }

    Component {
        id: checkboxFieldComp
        FormField {
            id: field
            label: parent.label
            error: parent.error
            info: parent.info

            function focusField() { checkbox.forceActiveFocus() }

            FormCheckbox {
                id: checkbox
                checked: field.parent.value === true
                hasError: field.error !== ""
                label: field.parent.fieldData && field.parent.fieldData.label
                       ? field.parent.fieldData.label : ""
                onToggled: root.formModel.setFieldValue(field.parent.index, checked)
                onActiveFocusChanged: {
                    if (activeFocus) root.formModel.fieldFocused(field.parent.index)
                    else root.formModel.fieldBlurred(field.parent.index)
                }
            }
        }
    }

    Component {
        id: dropdownFieldComp
        FormField {
            id: field
            label: parent.label
            error: parent.error
            info: parent.info

            function focusField() { dropdown.forceActiveFocus() }

            readonly property var _fd: parent.fieldData || ({})
            readonly property var _items: _fd.items || []

            function _findCurrentItem(items, value) {
                for (var s = 0; s < items.length; s++) {
                    var section = items[s]
                    if (!section || !section.items) continue
                    for (var i = 0; i < section.items.length; i++) {
                        if (section.items[i].id === value) return section.items[i]
                    }
                }
                return null
            }

            SearchableDropdown {
                id: dropdown
                items: field._items
                hasError: field.error !== ""
                currentItem: field._findCurrentItem(field._items, field.parent.value)
                placeholder: field._fd.placeholder || field.parent.placeholder || ""
                onActivated: (item) => {
                    root.formModel.setFieldValue(field.parent.index, item.id)
                }
                onActiveFocusChanged: {
                    if (activeFocus) root.formModel.fieldFocused(field.parent.index)
                    else root.formModel.fieldBlurred(field.parent.index)
                }
            }
        }
    }

    Component {
        id: filepickerFieldComp
        FormField {
            id: field
            label: parent.label
            error: parent.error
            info: parent.info

            function focusField() { filePicker.forceActiveFocus() }

            readonly property var _fd: parent.fieldData || ({})

            FormFilePicker {
                id: filePicker
                hasError: field.error !== ""
                multiple: field._fd.multiple || false
                directoriesOnly: field._fd.directoriesOnly || false
                selectedPaths: {
                    var v = field.parent.value
                    if (Array.isArray(v)) return v
                    if (typeof v === "string" && v !== "") return [v]
                    return []
                }
                onPathsChanged: (paths) => {
                    root.formModel.setFilePaths(field.parent.index, paths)
                }
            }
        }
    }

    Component {
        id: datepickerFieldComp
        FormField {
            id: field
            label: parent.label
            error: parent.error
            info: parent.info

            function focusField() { dateInput.forceActiveFocus() }

            readonly property var _fd: parent.fieldData || ({})

            FormDateInput {
                id: dateInput
                text: field.parent.value != null ? String(field.parent.value) : ""
                hasError: field.error !== ""
                includeTime: field._fd.includeTime || false
                minDate: field._fd.min || ""
                maxDate: field._fd.max || ""
                onTextEdited: root.formModel.setFieldValue(field.parent.index, text)
                onActiveFocusChanged: {
                    if (activeFocus) root.formModel.fieldFocused(field.parent.index)
                    else root.formModel.fieldBlurred(field.parent.index)
                }
            }
        }
    }

    Component {
        id: descriptionFieldComp
        FormField {
            label: parent.label
            error: ""
            info: ""

            readonly property var _fd: parent.fieldData || ({})

            Text {
                Layout.fillWidth: true
                text: _fd.text || ""
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
