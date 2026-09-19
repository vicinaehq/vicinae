pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

ViciModal {
    id: root

    required property AISettingsModel model

    property string providerType: ""
    property string providerLabel: ""
    property var providerIcon: null
    property bool allowMultiple: false
    property var _keys: []
    property var _values: ({})

    function openFor(type, label, icon, allowMultiple) {
        providerType = type;
        providerLabel = label;
        providerIcon = icon;
        root.allowMultiple = allowMultiple;
        _keys = root.model.prepareSetup(type);
        _values = {};
        idField.text = allowMultiple ? root.model.nextProviderId(type) : "";
        open();
    }

    function _setValue(key, value) {
        const next = Object.assign({}, _values);
        next[key] = value;
        _values = next;
    }

    readonly property bool _canSubmit: {
        if (providerType === "")
            return false;
        if (allowMultiple) {
            const id = idField.text.trim();
            if (id.length === 0 || root.model.isProviderIdTaken(id))
                return false;
        }
        for (let i = 0; i < _keys.length; i++) {
            const value = _values[_keys[i]];
            if (typeof value === "string" && value.trim().length === 0)
                return false;
        }
        return true;
    }

    function _submit() {
        if (!_canSubmit)
            return;
        const fields = Object.assign({}, _values);
        if (allowMultiple)
            fields.id = idField.text.trim();
        root.model.addProvider(providerType, fields);
        close();
    }

    parent: Overlay.overlay
    width: Math.min(parent.width - 40, 480)
    padding: 24

    onOpened: {
        if (allowMultiple)
            idField.forceActiveFocus();
    }

    contentItem: ColumnLayout {
        spacing: 20

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 8

            ViciImage {
                source: root.providerIcon ? root.providerIcon.withFillColor(Theme.foreground) : null
                Layout.preferredWidth: 36
                Layout.preferredHeight: 36
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: qsTr("Set up %1").arg(root.providerLabel)
                color: Theme.foreground
                font.pointSize: Theme.regularFontSize + 2
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }
        }

        ColumnLayout {
            visible: root.allowMultiple
            Layout.fillWidth: true
            spacing: 6

            Text {
                text: qsTr("Instance ID")
                color: Theme.foreground
                font.pointSize: Theme.regularFontSize
            }

            Text {
                text: qsTr("A unique identifier for this instance.")
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
            }

            FormTextInput {
                id: idField
                Layout.fillWidth: true
                hasError: text.trim().length > 0 && root.model.isProviderIdTaken(text.trim())
                onAccepted: root._submit()
            }

            Text {
                visible: idField.hasError
                text: qsTr("This instance ID is already in use.")
                color: Theme.danger
                font.pointSize: Theme.smallerFontSize
            }
        }

        Repeater {
            model: root.model.setupFields

            delegate: ColumnLayout {
                id: field
                required property string key
                required property string label
                required property string description
                required property string placeholder
                required property string kind
                required property var options
                required property var value
                required property var currentOption
                Layout.fillWidth: true
                spacing: 6

                Component.onCompleted: root._setValue(key, value)

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 6

                        Text {
                            text: field.label
                            color: Theme.foreground
                            font.pointSize: Theme.regularFontSize
                        }

                        Text {
                            text: field.description
                            color: Theme.textMuted
                            font.pointSize: Theme.smallerFontSize
                            wrapMode: Text.Wrap
                            Layout.fillWidth: true
                        }
                    }

                    Item {
                        visible: field.kind === "toggle"
                        Layout.preferredWidth: toggle.implicitWidth
                        Layout.preferredHeight: toggle.implicitHeight

                        SettingsToggle {
                            id: toggle
                            checked: field.value === true
                            accessibleLabel: field.label
                            onToggled: checked => {
                                toggle.checked = checked;
                                root._setValue(field.key, checked);
                            }
                        }
                    }
                }

                FormTextInput {
                    visible: field.kind === "text"
                    Layout.fillWidth: true
                    placeholder: field.placeholder
                    text: field.kind === "text" ? String(field.value) : ""
                    onTextChanged: root._setValue(field.key, text)
                    onAccepted: root._submit()
                }

                FormPasswordInput {
                    visible: field.kind === "secret"
                    placeholder: field.placeholder
                    Component.onCompleted: {
                        if (field.kind === "secret")
                            forceActiveFocus();
                    }
                    onTextChanged: root._setValue(field.key, text)
                    onAccepted: root._submit()
                }

                SearchableDropdown {
                    visible: field.kind === "select"
                    Layout.fillWidth: true
                    items: field.options
                    currentItem: field.currentOption
                    onActivated: item => root._setValue(field.key, item.id)
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: 4
            spacing: 10

            ViciButton {
                Layout.fillWidth: true
                implicitHeight: 32
                variant: "ghost"
                bordered: true
                text: qsTr("Cancel")
                activeFocusOnTab: true
                onClicked: root.close()
            }

            ViciButton {
                Layout.fillWidth: true
                implicitHeight: 32
                variant: "accent"
                text: qsTr("Add")
                activeFocusOnTab: true
                enabled: root._canSubmit
                opacity: root._canSubmit ? 1.0 : 0.5
                onClicked: root._submit()
            }
        }
    }
}
