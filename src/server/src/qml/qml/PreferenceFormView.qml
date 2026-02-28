import QtQuick
import QtQuick.Layouts

Item {
    id: root
    required property var prefModel
    implicitHeight: formView.contentHeight

    FormView {
        id: formView
        anchors.fill: parent

        Repeater {
            id: repeater
            model: root.prefModel

            delegate: Loader {
                id: fieldLoader
                Layout.fillWidth: true

                required property int index
                required property string type
                required property string fieldId
                required property string label
                required property string description
                required property string placeholder
                required property var value
                required property var options
                required property bool readOnly
                required property bool multiple
                required property bool directoriesOnly

                sourceComponent: {
                    switch (type) {
                    case "text": return textComp
                    case "password": return passwordComp
                    case "checkbox": return checkboxComp
                    case "dropdown": return dropdownComp
                    case "filepicker":
                    case "directorypicker": return filepickerComp
                    default: return null
                    }
                }
            }
        }
    }

    Component {
        id: textComp
        FormField {
            id: field
            label: parent.label
            info: parent.description
            FormTextInput {
                text: field.parent.value != null ? String(field.parent.value) : ""
                placeholder: field.parent.placeholder
                readOnly: field.parent.readOnly
                onTextEdited: root.prefModel.setFieldValue(field.parent.index, text)
            }
        }
    }

    Component {
        id: passwordComp
        FormField {
            id: field
            label: parent.label
            info: parent.description
            FormTextInput {
                text: field.parent.value != null ? String(field.parent.value) : ""
                placeholder: field.parent.placeholder
                readOnly: field.parent.readOnly
                echoMode: TextInput.Password
                onTextEdited: root.prefModel.setFieldValue(field.parent.index, text)
            }
        }
    }

    Component {
        id: checkboxComp
        FormField {
            id: field
            label: parent.label
            info: parent.description
            FormCheckbox {
                checked: field.parent.value === true
                readOnly: field.parent.readOnly
                onToggled: root.prefModel.setFieldValue(field.parent.index, checked)
            }
        }
    }

    Component {
        id: dropdownComp
        FormField {
            id: field
            label: parent.label
            info: parent.description

            function _findCurrentItem(items, val) {
                for (var s = 0; s < items.length; s++) {
                    var section = items[s]
                    if (!section || !section.items) continue
                    for (var i = 0; i < section.items.length; i++) {
                        if (section.items[i].id === val) return section.items[i]
                    }
                }
                return null
            }

            SearchableDropdown {
                items: field.parent.options || []
                readOnly: field.parent.readOnly
                currentItem: field._findCurrentItem(field.parent.options || [], field.parent.value)
                onActivated: (item) => root.prefModel.setFieldValue(field.parent.index, item.id)
            }
        }
    }

    Component {
        id: filepickerComp
        FormField {
            id: field
            label: parent.label
            info: parent.description
            FormFilePicker {
                multiple: field.parent.multiple
                directoriesOnly: field.parent.directoriesOnly
                readOnly: field.parent.readOnly
                selectedPaths: {
                    var v = field.parent.value
                    if (Array.isArray(v)) return v
                    if (typeof v === "string" && v !== "") return [v]
                    return []
                }
                onPathsChanged: (paths) => {
                    if (field.parent.multiple) root.prefModel.setFieldValue(field.parent.index, paths)
                    else root.prefModel.setFieldValue(field.parent.index, paths.length > 0 ? paths[0] : "")
                }
            }
        }
    }
}
