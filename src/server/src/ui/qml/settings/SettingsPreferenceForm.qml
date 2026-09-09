pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

ColumnLayout {
    id: root
    required property PreferenceFormModel prefModel
    // Shared width for every field's control, so the label/description column
    // is the same width across all rows regardless of control type.
    property real fieldControlWidth: 300
    spacing: 0

    Repeater {
        id: settingsRepeater
        model: root.prefModel

        delegate: FieldHost {}
    }

    component FieldHost: Loader {
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
        required property list<string> lockedPaths

        sourceComponent: {
            switch (type) {
            case "text":
                return textComp;
            case "password":
                return passwordComp;
            case "checkbox":
                return switchComp;
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
        SettingsRow {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            description: field.host.description
            controlWidth: root.fieldControlWidth
            showSeparator: field.host.index < settingsRepeater.count - 1

            FormTextInput {
                width: parent.width
                releaseFocusOnAccept: true
                text: field.host.value != null ? String(field.host.value) : ""
                placeholder: field.host.placeholder
                readOnly: field.host.readOnly
                onTextEdited: root.prefModel.setFieldValue(field.host.index, text)
            }
        }
    }

    Component {
        id: passwordComp
        SettingsRow {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            description: field.host.description
            controlWidth: root.fieldControlWidth
            showSeparator: field.host.index < settingsRepeater.count - 1

            property bool revealed: false

            RowLayout {
                width: parent.width
                spacing: 6

                FormTextInput {
                    Layout.fillWidth: true
                    releaseFocusOnAccept: true
                    text: field.host.value != null ? String(field.host.value) : ""
                    placeholder: field.host.placeholder
                    readOnly: field.host.readOnly
                    echoMode: field.revealed ? TextInput.Normal : TextInput.Password
                    onTextEdited: root.prefModel.setFieldValue(field.host.index, text)
                }
                ViciButton {
                    id: revealBtn
                    Layout.preferredWidth: 36
                    Layout.preferredHeight: 36
                    radius: 8
                    iconSource: Img.builtin(field.revealed ? "eye-disabled" : "eye").withFillColor(Theme.textMuted)
                    variant: "ghost"
                    border.width: revealBtn.hovered ? 1 : 0
                    border.color: Config.withAlpha(Theme.inputBorder, Config.surfaceOpacity)
                    onClicked: field.revealed = !field.revealed
                }
            }
        }
    }

    Component {
        id: switchComp
        SettingsRow {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label !== "" ? field.host.label : field.host.checkboxLabel
            description: field.host.description
            controlWidth: root.fieldControlWidth
            showSeparator: field.host.index < settingsRepeater.count - 1

            SettingsToggle {
                opacity: field.host.readOnly ? 0.5 : 1.0
                checked: field.host.value === true
                onToggled: checked => {
                    if (field.host.readOnly)
                        return;
                    root.prefModel.setFieldValue(field.host.index, checked);
                }
            }
        }
    }

    Component {
        id: dropdownComp
        SettingsRow {
            id: field
            readonly property FieldHost host: parent as FieldHost
            label: field.host.label
            description: field.host.description
            controlWidth: root.fieldControlWidth
            showSeparator: field.host.index < settingsRepeater.count - 1

            SearchableDropdown {
                width: parent.width
                model: field.host.dropdownModel
                readOnly: field.host.readOnly
                currentItem: field.host.currentDropdownItem
                onActivated: item => root.prefModel.setFieldValue(field.host.index, item.id)
            }
        }
    }

    // File/directory pickers use a vertical layout: a fixed-width slot on the
    // right doesn't work well as the selected-path list grows.
    Component {
        id: filepickerComp
        ColumnLayout {
            id: field
            readonly property FieldHost host: parent as FieldHost
            Layout.fillWidth: true
            spacing: 0

            ColumnLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                Layout.topMargin: 14
                Layout.bottomMargin: 14
                spacing: 8

                Text {
                    text: field.host.label
                    color: Theme.foreground
                    font.pointSize: Theme.regularFontSize
                    Layout.fillWidth: true
                }

                Text {
                    visible: field.host.description !== ""
                    text: field.host.description
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }

                FormFilePicker {
                    Layout.fillWidth: true
                    Layout.topMargin: 2
                    multiple: field.host.multiple
                    canChooseFiles: field.host.canChooseFiles
                    canChooseDirectories: field.host.canChooseDirectories
                    readOnly: field.host.readOnly
                    lockedPaths: field.host.lockedPaths
                    selectedPaths: {
                        const v = field.host.value;
                        if (!v)
                            return [];
                        if (typeof v === "string")
                            return v !== "" ? [v] : [];
                        let arr = [];
                        for (let i = 0; i < v.length; i++)
                            arr.push(v[i]);
                        return arr;
                    }
                    onPathsChanged: paths => {
                        if (field.host.multiple)
                            root.prefModel.setFieldValue(field.host.index, paths);
                        else
                            root.prefModel.setFieldValue(field.host.index, paths.length > 0 ? paths[0] : "");
                    }
                }
            }

            ViciDivider {
                visible: field.host.index < settingsRepeater.count - 1
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16
            }
        }
    }
}
