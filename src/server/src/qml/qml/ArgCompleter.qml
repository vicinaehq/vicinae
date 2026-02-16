import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root

    required property var args        // launcher.completerArgs
    required property string icon     // launcher.completerIcon

    signal valueChanged(int index, string value)
    signal focusSearchInput()

    readonly property int maxArgs: 3
    readonly property var visibleArgs: args ? args.slice(0, maxArgs) : []

    spacing: 4

    function validate() {
        var firstRequired = -1
        for (var i = 0; i < argRepeater.count; i++) {
            var loader = argRepeater.itemAt(i)
            if (!loader || !loader.item) continue
            var arg = root.visibleArgs[i]
            if (arg.required && loader.item.currentValue === "") {
                loader.item.showError = true
                if (firstRequired === -1) {
                    firstRequired = i
                    loader.item.forceActiveFocus()
                }
            }
        }
    }

    function setValues(values) {
        for (var i = 0; i < argRepeater.count && i < values.length; i++) {
            var loader = argRepeater.itemAt(i)
            if (!loader || !loader.item) continue
            var val = values[i].value
            if (loader.item.currentValue !== val)
                loader.item.currentValue = val
        }
    }

    Image {
        source: root.icon
        sourceSize.width: 20
        sourceSize.height: 20
        Layout.preferredWidth: 20
        Layout.preferredHeight: 20
        Layout.alignment: Qt.AlignVCenter
        asynchronous: true
    }

    Repeater {
        id: argRepeater
        model: root.visibleArgs

        delegate: Loader {
            id: argLoader
            required property int index
            required property var modelData

            readonly property bool isLast: index === root.visibleArgs.length - 1

            Layout.alignment: Qt.AlignVCenter

            sourceComponent: modelData.type === "dropdown" ? dropdownDelegate : textDelegate

            Component {
                id: textDelegate

                Rectangle {
                    id: textDel
                    property string currentValue: textField.text
                    property bool showError: false

                    implicitWidth: textMetrics.advanceWidth + 16
                    implicitHeight: 26
                    radius: 4
                    color: "transparent"
                    border.width: 1
                    border.color: textDel.showError ? "#e53935"
                                : textField.activeFocus ? Theme.accent
                                : Theme.divider

                    function forceActiveFocus() { textField.forceActiveFocus() }

                    onCurrentValueChanged: {
                        if (textField.text !== currentValue)
                            textField.text = currentValue
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
                        font.pointSize: Theme.regularFontSize
                        color: Theme.foreground
                        clip: true
                        activeFocusOnTab: true
                        echoMode: argLoader.modelData.type === "password"
                                  ? TextInput.Password : TextInput.Normal

                        Text {
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter
                            text: argLoader.modelData.placeholder || ""
                            color: Theme.textPlaceholder
                            font: textField.font
                            visible: !textField.text && textField.echoMode !== TextInput.Password
                        }

                        onTextEdited: {
                            textDel.showError = false
                            root.valueChanged(argLoader.index, text)
                        }

                        Keys.onUpPressed: {
                            if (commandStack.currentItem) commandStack.currentItem.moveUp()
                            else if (launcher.hasCommandView) launcher.forwardKey(Qt.Key_Up)
                            else searchList.moveUp()
                        }
                        Keys.onDownPressed: {
                            if (commandStack.currentItem) commandStack.currentItem.moveDown()
                            else if (launcher.hasCommandView) launcher.forwardKey(Qt.Key_Down)
                            else searchList.moveDown()
                        }
                        Keys.onReturnPressed: (event) => {
                            if (event.modifiers !== Qt.NoModifier) {
                                event.accepted = launcher.forwardKey(event.key, event.modifiers)
                            } else {
                                launcher.handleReturn()
                            }
                        }
                        Keys.onTabPressed: (event) => {
                            if (argLoader.isLast) {
                                root.focusSearchInput()
                                event.accepted = true
                            } else {
                                event.accepted = false
                            }
                        }
                        Keys.onPressed: (event) => {
                            if (event.modifiers !== Qt.NoModifier && event.modifiers !== Qt.ShiftModifier
                                    && event.key !== Qt.Key_Shift && event.key !== Qt.Key_Control
                                    && event.key !== Qt.Key_Alt && event.key !== Qt.Key_Meta) {
                                event.accepted = launcher.forwardKey(event.key, event.modifiers)
                            } else {
                                event.accepted = false
                            }
                        }
                    }
                }
            }

            Component {
                id: dropdownDelegate

                Rectangle {
                    id: dropdownDel
                    property string currentValue: ""
                    property bool showError: false

                    implicitWidth: Math.max(dropdownMetrics.advanceWidth + 36, 80)
                    implicitHeight: 26
                    radius: 4
                    color: "transparent"
                    border.width: 1
                    border.color: dropdownDel.showError ? "#e53935"
                                : dropdown.activeFocus ? Theme.accent
                                : Theme.divider

                    function forceActiveFocus() { dropdown.forceActiveFocus() }

                    onCurrentValueChanged: {
                        if (!argLoader.modelData.data) return
                        for (var i = 0; i < argLoader.modelData.data.length; i++) {
                            if (argLoader.modelData.data[i].value === currentValue) {
                                dropdown.currentItem = {
                                    id: argLoader.modelData.data[i].value,
                                    displayName: argLoader.modelData.data[i].title
                                }
                                return
                            }
                        }
                    }

                    TextMetrics {
                        id: dropdownMetrics
                        font.pointSize: Theme.regularFontSize
                        text: dropdown.currentItem
                              ? dropdown.currentItem.displayName
                              : (argLoader.modelData.placeholder || " ")
                    }

                    SearchableDropdown {
                        id: dropdown
                        anchors.fill: parent
                        compact: true
                        placeholder: argLoader.modelData.placeholder || ""
                        items: {
                            if (!argLoader.modelData.data) return []
                            var entries = []
                            for (var i = 0; i < argLoader.modelData.data.length; i++) {
                                var d = argLoader.modelData.data[i]
                                entries.push({ id: d.value, displayName: d.title, iconSource: "" })
                            }
                            return [{ title: "", items: entries }]
                        }
                        onActivated: (item) => {
                            dropdown.currentItem = item
                            dropdownDel.showError = false
                            dropdownDel.currentValue = item.id
                            root.valueChanged(argLoader.index, item.id)
                        }
                    }
                }
            }
        }
    }

    // Absorb remaining space so inputs pack to the left and grow rightward
    Item { Layout.fillWidth: true }
}
