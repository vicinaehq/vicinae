import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: root
    required property var prefModel
    spacing: 20

    Repeater {
        model: root.prefModel

        delegate: Loader {
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
                case "checkbox": return switchComp
                case "dropdown": return dropdownComp
                case "filepicker":
                case "directorypicker": return filepickerComp
                default: return null
                }
            }
        }
    }

    Component {
        id: textComp
        ColumnLayout {
            id: field
            spacing: 6
            Text {
                text: field.parent.label
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
            }
            FormTextInput {
                Layout.fillWidth: true
                text: field.parent.value != null ? String(field.parent.value) : ""
                placeholder: field.parent.placeholder
                readOnly: field.parent.readOnly
                onTextEdited: root.prefModel.setFieldValue(field.parent.index, text)
            }
            Text {
                visible: field.parent.description !== ""
                text: field.parent.description
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }
        }
    }

    Component {
        id: passwordComp
        ColumnLayout {
            id: field
            spacing: 6
            Text {
                text: field.parent.label
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
            }
            FormTextInput {
                Layout.fillWidth: true
                text: field.parent.value != null ? String(field.parent.value) : ""
                placeholder: field.parent.placeholder
                readOnly: field.parent.readOnly
                echoMode: TextInput.Password
                onTextEdited: root.prefModel.setFieldValue(field.parent.index, text)
            }
            Text {
                visible: field.parent.description !== ""
                text: field.parent.description
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }
        }
    }

    Component {
        id: switchComp
        ColumnLayout {
            id: field
            spacing: 6

            RowLayout {
                Layout.fillWidth: true

                Text {
                    text: field.parent.label
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize
                    Layout.fillWidth: true
                }

                Item {
                    id: toggle
                    property bool checked: field.parent.value === true
                    opacity: field.parent.readOnly ? 0.5 : 1.0
                    width: 36
                    height: 20

                    Rectangle {
                        anchors.fill: parent
                        radius: 10
                        color: toggle.checked ? Theme.accent
                               : Qt.rgba(Theme.foreground.r, Theme.foreground.g,
                                         Theme.foreground.b, 0.2)
                        Behavior on color { ColorAnimation { duration: 120 } }

                        Rectangle {
                            width: 16
                            height: 16
                            radius: 8
                            x: toggle.checked ? parent.width - width - 2 : 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: "#ffffff"
                            Behavior on x { NumberAnimation { duration: 120 } }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: field.parent.readOnly ? Qt.ArrowCursor : Qt.PointingHandCursor
                        onClicked: {
                            if (field.parent.readOnly) return
                            toggle.checked = !toggle.checked
                            root.prefModel.setFieldValue(field.parent.index, toggle.checked)
                        }
                    }
                }
            }

            Text {
                visible: field.parent.description !== ""
                text: field.parent.description
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }
        }
    }

    Component {
        id: dropdownComp
        ColumnLayout {
            id: field
            spacing: 6

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

            Text {
                text: field.parent.label
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
            }
            SearchableDropdown {
                Layout.fillWidth: true
                items: field.parent.options || []
                readOnly: field.parent.readOnly
                currentItem: field._findCurrentItem(
                    field.parent.options || [], field.parent.value)
                onActivated: (item) => root.prefModel.setFieldValue(field.parent.index, item.id)
            }
            Text {
                visible: field.parent.description !== ""
                text: field.parent.description
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }
        }
    }

    Component {
        id: filepickerComp
        ColumnLayout {
            id: field
            spacing: 6
            Text {
                text: field.parent.label
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
            }
            FormFilePicker {
                Layout.fillWidth: true
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
            Text {
                visible: field.parent.description !== ""
                text: field.parent.description
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }
        }
    }
}
