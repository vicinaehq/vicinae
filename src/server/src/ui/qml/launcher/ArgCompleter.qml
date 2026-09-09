pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

RowLayout {
    id: root

    required property StackView commandStack
    required property var args
    required property string icon

    signal valueChanged(int index, string value)
    signal focusSearchInput

    readonly property int maxArgs: 3
    readonly property var visibleArgs: args ? args.slice(0, maxArgs) : []

    spacing: 4

    component ArgField: Rectangle {
        property string currentValue: ""
        property bool showError: false
    }

    function focusFirst() {
        ((argRepeater.itemAt(0) as Loader)?.item as Item)?.forceActiveFocus();
    }

    function validate() {
        var firstRequired = -1;
        for (var i = 0; i < argRepeater.count; i++) {
            var field = (argRepeater.itemAt(i) as Loader)?.item as ArgField;
            if (!field)
                continue;
            var arg = root.visibleArgs[i];
            if (arg.required && field.currentValue === "") {
                field.showError = true;
                if (firstRequired === -1) {
                    firstRequired = i;
                    field.forceActiveFocus();
                }
            }
        }
    }

    function setValues(values) {
        for (var i = 0; i < argRepeater.count && i < values.length; i++) {
            var field = (argRepeater.itemAt(i) as Loader)?.item as ArgField;
            if (!field)
                continue;
            var val = values[i].value;
            if (field.currentValue !== val)
                field.currentValue = val;
        }
    }

    ViciImage {
        Layout.preferredWidth: 25
        Layout.preferredHeight: 25
        Layout.alignment: Qt.AlignVCenter
        source: root.icon
    }

    Repeater {
        id: argRepeater
        model: root.visibleArgs

        delegate: Loader {
            id: argLoader
            required property int index
            required property var modelData

            readonly property bool isLast: index === root.visibleArgs.length - 1
            readonly property real maxArgWidth: {
                var totalSpacing = root.spacing * (root.visibleArgs.length + 1);
                return Math.max((root.width - 25 - totalSpacing) / root.visibleArgs.length, 60);
            }

            Layout.alignment: Qt.AlignVCenter

            sourceComponent: modelData.type === "dropdown" ? dropdownDelegate : textDelegate

            Component {
                id: textDelegate

                ArgField {
                    id: textDel
                    currentValue: textField.text

                    implicitWidth: Math.min((textField.text ? textField.contentWidth : textMetrics.advanceWidth) + 16, argLoader.maxArgWidth)
                    implicitHeight: 26
                    radius: 4
                    color: "transparent"
                    border.width: 1
                    border.color: Config.withAlpha(textDel.showError ? Theme.inputBorderError : textField.activeFocus ? Theme.inputBorderFocus : Theme.divider, Config.windowOpacity)

                    function forceActiveFocus() {
                        textField.forceActiveFocus();
                    }

                    onCurrentValueChanged: {
                        if (textField.text !== currentValue)
                            textField.text = currentValue;
                    }

                    TextMetrics {
                        id: textMetrics
                        font: textField.font
                        text: textField.text || argLoader.modelData.placeholder || " "
                    }

                    TextInput {
                        id: textField
                        anchors.fill: parent
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        verticalAlignment: TextInput.AlignVCenter
                        font.family: Theme.fontFamily
                        font.pointSize: Theme.regularFontSize
                        color: Theme.foreground
                        clip: true
                        activeFocusOnTab: true
                        echoMode: argLoader.modelData.type === "password" ? TextInput.Password : TextInput.Normal

                        Text {
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter
                            text: argLoader.modelData.placeholder || ""
                            color: Theme.textPlaceholder
                            font: textField.font
                            visible: !textField.text && textField.echoMode !== TextInput.Password
                        }

                        onTextEdited: {
                            textDel.showError = false;
                            root.valueChanged(argLoader.index, text);
                        }

                        Keys.onUpPressed: {
                            // qmllint disable missing-property
                            root.commandStack.currentItem.moveUp();
                        }
                        Keys.onDownPressed: {
                            root.commandStack.currentItem.moveDown();
                            // qmllint enable missing-property
                        }
                        Keys.onTabPressed: event => {
                            if (argLoader.isLast) {
                                root.focusSearchInput();
                                event.accepted = true;
                            } else {
                                event.accepted = false;
                            }
                        }
                        Keys.onPressed: event => {
                            event.accepted = Launcher.forwardKey(event.key, event.modifiers, event.nativeScanCode);
                        }
                    }
                }
            }

            Component {
                id: dropdownDelegate

                ArgField {
                    id: dropdownDel

                    implicitWidth: Math.min(Math.max(dropdownMetrics.advanceWidth + 36, 80), argLoader.maxArgWidth)
                    implicitHeight: 26
                    radius: 4
                    color: "transparent"
                    border.width: 1
                    border.color: "transparent"

                    function forceActiveFocus() {
                        dropdown.forceActiveFocus();
                    }

                    onCurrentValueChanged: {
                        if (!argLoader.modelData.data)
                            return;
                        for (var i = 0; i < argLoader.modelData.data.length; i++) {
                            if (argLoader.modelData.data[i].value === currentValue) {
                                dropdown.currentItem = {
                                    id: argLoader.modelData.data[i].value,
                                    displayName: argLoader.modelData.data[i].title
                                };
                                return;
                            }
                        }
                    }

                    TextMetrics {
                        id: dropdownMetrics
                        font.family: Theme.fontFamily
                        font.pointSize: Theme.regularFontSize
                        text: dropdown.currentItem ? dropdown.currentItem.displayName : (argLoader.modelData.placeholder || " ")
                    }

                    SearchableDropdown {
                        id: dropdown
                        anchors.fill: parent
                        compact: true
                        activeFocusOnTab: true
                        placeholder: argLoader.modelData.placeholder || ""
                        items: {
                            if (!argLoader.modelData.data)
                                return [];
                            var entries = [];
                            for (var i = 0; i < argLoader.modelData.data.length; i++) {
                                var d = argLoader.modelData.data[i];
                                entries.push({
                                    id: d.value,
                                    displayName: d.title,
                                    iconSource: ""
                                });
                            }
                            return [
                                {
                                    title: "",
                                    items: entries
                                }
                            ];
                        }
                        onActivated: item => {
                            dropdown.currentItem = item;
                            dropdownDel.showError = false;
                            dropdownDel.currentValue = item.id;
                            root.valueChanged(argLoader.index, item.id);
                        }

                        Keys.onTabPressed: event => {
                            if (argLoader.isLast) {
                                root.focusSearchInput();
                                event.accepted = true;
                            } else {
                                event.accepted = false;
                            }
                        }
                        Keys.onPressed: event => {
                            event.accepted = Launcher.forwardKey(event.key, event.modifiers, event.nativeScanCode);
                        }
                    }
                }
            }
        }
    }

    Item {
        Layout.fillWidth: true
    }
}
