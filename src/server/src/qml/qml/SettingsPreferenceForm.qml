import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: root
    required property var prefModel
    // Shared width for every field's control, so the label/description column
    // is the same width across all rows regardless of control type.
    property real fieldControlWidth: 300
    spacing: 0

    Repeater {
        id: settingsRepeater
        model: root.prefModel

        delegate: Loader {
            Layout.fillWidth: true

            required property int index
            required property string type
            required property string fieldId
            required property string label
            required property string checkboxLabel
            required property string description
            required property string placeholder
            required property var value
            required property var options
            required property bool readOnly
            required property bool multiple
            required property bool canChooseFiles
            required property bool canChooseDirectories

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
    }

    Component {
        id: textComp
        SettingsRow {
            id: field
            label: field.parent.label
            description: field.parent.description
            controlWidth: root.fieldControlWidth
            showSeparator: field.parent.index < settingsRepeater.count - 1

            FormTextInput {
                width: parent.width
                releaseFocusOnAccept: true
                text: field.parent.value != null ? String(field.parent.value) : ""
                placeholder: field.parent.placeholder
                readOnly: field.parent.readOnly
                onTextEdited: root.prefModel.setFieldValue(field.parent.index, text)
            }
        }
    }

    Component {
        id: passwordComp
        SettingsRow {
            id: field
            label: field.parent.label
            description: field.parent.description
            controlWidth: root.fieldControlWidth
            showSeparator: field.parent.index < settingsRepeater.count - 1

            property bool revealed: false

            RowLayout {
                width: parent.width
                spacing: 6

                FormTextInput {
                    Layout.fillWidth: true
                    releaseFocusOnAccept: true
                    text: field.parent.value != null ? String(field.parent.value) : ""
                    placeholder: field.parent.placeholder
                    readOnly: field.parent.readOnly
                    echoMode: field.revealed ? TextInput.Normal : TextInput.Password
                    onTextEdited: root.prefModel.setFieldValue(field.parent.index, text)
                }
                ViciButton {
                    id: revealBtn
                    Layout.preferredWidth: 36
                    Layout.preferredHeight: 36
                    radius: 8
                    iconSource: Img.builtin(field.revealed ? "eye-disabled" : "eye").withFillColor(Theme.textMuted)
                    variant: "ghost"
                    border.width: revealBtn.hovered ? 1 : 0
                    border.color: Config.withAlpha(Theme.inputBorder, Config.windowOpacity)
                    onClicked: field.revealed = !field.revealed
                }
            }
        }
    }

    Component {
        id: switchComp
        SettingsRow {
            id: field
            label: field.parent.label !== "" ? field.parent.label : field.parent.checkboxLabel
            description: field.parent.description
            controlWidth: root.fieldControlWidth
            showSeparator: field.parent.index < settingsRepeater.count - 1

            SettingsToggle {
                opacity: field.parent.readOnly ? 0.5 : 1.0
                checked: field.parent.value === true
                onToggled: checked => {
                    if (field.parent.readOnly)
                        return;
                    root.prefModel.setFieldValue(field.parent.index, checked);
                }
            }
        }
    }

    Component {
        id: dropdownComp
        SettingsRow {
            id: field
            label: field.parent.label
            description: field.parent.description
            controlWidth: root.fieldControlWidth
            showSeparator: field.parent.index < settingsRepeater.count - 1

            function _findCurrentItem(items, val) {
                for (var s = 0; s < items.length; s++) {
                    var section = items[s];
                    if (!section || !section.items)
                        continue;
                    for (var i = 0; i < section.items.length; i++) {
                        if (section.items[i].id === val)
                            return section.items[i];
                    }
                }
                return null;
            }

            SearchableDropdown {
                width: parent.width
                items: field.parent.options || []
                readOnly: field.parent.readOnly
                currentItem: field._findCurrentItem(field.parent.options || [], field.parent.value)
                onActivated: item => root.prefModel.setFieldValue(field.parent.index, item.id)
            }
        }
    }

    // File/directory pickers use a vertical layout: a fixed-width slot on the
    // right doesn't work well as the selected-path list grows.
    Component {
        id: filepickerComp
        ColumnLayout {
            id: field
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
                    text: field.parent.label
                    color: Theme.foreground
                    font.pointSize: Theme.regularFontSize
                    Layout.fillWidth: true
                }

                Text {
                    visible: field.parent.description !== ""
                    text: field.parent.description
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }

                FormFilePicker {
                    Layout.fillWidth: true
                    Layout.topMargin: 2
                    multiple: field.parent.multiple
                    canChooseFiles: field.parent.canChooseFiles
                    canChooseDirectories: field.parent.canChooseDirectories
                    readOnly: field.parent.readOnly
                    selectedPaths: {
                        const v = field.parent.value;
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
                        if (field.parent.multiple)
                            root.prefModel.setFieldValue(field.parent.index, paths);
                        else
                            root.prefModel.setFieldValue(field.parent.index, paths.length > 0 ? paths[0] : "");
                    }
                }
            }

            ViciDivider {
                visible: field.parent.index < settingsRepeater.count - 1
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16
            }
        }
    }
}
