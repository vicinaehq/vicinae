pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root
    required property MissingPreferenceViewHost host

    FormView {
        id: formView
        anchors.fill: parent

        ColumnLayout {
            Layout.fillWidth: true
            Layout.margins: 20
            Layout.minimumWidth: 300
            Layout.maximumWidth: 500
            Layout.alignment: Qt.AlignHCenter
            spacing: 8

            ViciImage {
                source: root.host.commandIconSource
                Layout.preferredWidth: 32
                Layout.preferredHeight: 32
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: qsTr("Welcome to %1").arg(root.host.commandName)
                color: Theme.foreground
                font.pointSize: Theme.regularFontSize
                font.weight: Font.DemiBold
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            Text {
                text: qsTr("Before you can use this command, you need to fill in the required preference fields below.")
                color: Theme.textMuted
                font.pointSize: Theme.regularFontSize
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }
        }

        Repeater {
            id: missingRepeater
            model: root.host.prefModel

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
        required property string checkboxLabel
        required property string description
        required property string placeholder
        required property var value
        required property CompletionModel dropdownModel
        required property var currentDropdownItem
        required property bool readOnly
        required property bool multiple
        required property bool canChooseFiles
        required property bool canChooseDirectories

        onLoaded: if (index === 0)
            Qt.callLater(formView.focusFirst)

        sourceComponent: {
            switch (type) {
            case "text":
                return textComp;
            case "password":
                return passwordComp;
            case "checkbox":
                return checkboxComp;
            case "dropdown":
                return dropdownComp;
            case "filepicker":
            case "directorypicker":
                return filepickerComp;
            default:
                return null;
            }
        }
    }

    Component {
        id: textComp
        FormField {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            info: field.host.description
            FormTextInput {
                text: field.host.value != null ? String(field.host.value) : ""
                placeholder: field.host.placeholder
                onTextEdited: root.host.prefModel.setFieldValue(field.host.index, text)
            }
        }
    }

    Component {
        id: passwordComp
        FormField {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            info: field.host.description
            FormTextInput {
                text: field.host.value != null ? String(field.host.value) : ""
                placeholder: field.host.placeholder
                echoMode: TextInput.Password
                onTextEdited: root.host.prefModel.setFieldValue(field.host.index, text)
            }
        }
    }

    Component {
        id: checkboxComp
        FormField {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            FormCheckbox {
                checked: field.host.value === true
                label: field.host.checkboxLabel
                onToggled: root.host.prefModel.setFieldValue(field.host.index, checked)
            }
        }
    }

    Component {
        id: dropdownComp
        FormField {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            info: field.host.description

            SearchableDropdown {
                model: field.host.dropdownModel
                currentItem: field.host.currentDropdownItem
                onActivated: item => root.host.prefModel.setFieldValue(field.host.index, item.id)
            }
        }
    }

    Component {
        id: filepickerComp
        FormField {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            info: field.host.description
            topAlignLabel: missingFilePicker.multiple

            FormFilePicker {
                id: missingFilePicker
                multiple: field.host.multiple
                canChooseFiles: field.host.canChooseFiles
                canChooseDirectories: field.host.canChooseDirectories
                selectedPaths: {
                    var v = field.host.value;
                    if (Array.isArray(v))
                        return v;
                    if (typeof v === "string" && v !== "")
                        return [v];
                    return [];
                }
                onPathsChanged: paths => {
                    if (field.host.multiple)
                        root.host.prefModel.setFieldValue(field.host.index, paths);
                    else
                        root.host.prefModel.setFieldValue(field.host.index, paths.length > 0 ? paths[0] : "");
                }
            }
        }
    }
}
