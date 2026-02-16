import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root

    required property var args        // launcher.completerArgs
    required property string icon     // launcher.completerIcon

    signal valueChanged(int index, string value)
    signal focusSearchInput()

    spacing: 0

    function validate() {
        var firstRequired = -1
        for (var i = 0; i < argRepeater.count; i++) {
            var loader = argRepeater.itemAt(i)
            if (!loader || !loader.item) continue
            var arg = root.args[i]
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
        Layout.leftMargin: 8
        Layout.rightMargin: 4
        asynchronous: true
    }

    Repeater {
        id: argRepeater
        model: root.args

        delegate: Loader {
            id: argLoader
            required property int index
            required property var modelData

            readonly property bool isLast: index === root.args.length - 1

            Layout.alignment: Qt.AlignVCenter

            sourceComponent: modelData.type === "dropdown" ? dropdownDelegate : textDelegate

            Component {
                id: textDelegate

                Item {
                    id: textDel
                    property string currentValue: textField.text
                    property bool showError: false

                    implicitWidth: textMetrics.advanceWidth + 4
                    implicitHeight: 28

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

                    Rectangle {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        height: 1
                        color: textDel.showError ? "#e53935"
                             : textField.activeFocus ? Theme.accent
                             : Theme.divider
                    }

                    TextInput {
                        id: textField
                        anchors.fill: parent
                        anchors.leftMargin: 2
                        anchors.rightMargin: 2
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

                        Keys.onReturnPressed: (event) => {
                            event.accepted = false
                        }
                        Keys.onTabPressed: (event) => {
                            if (argLoader.isLast) {
                                root.focusSearchInput()
                                event.accepted = true
                            } else {
                                event.accepted = false
                            }
                        }
                    }
                }
            }

            Component {
                id: dropdownDelegate

                Item {
                    id: dropdownDel
                    property string currentValue: ""
                    property bool showError: false

                    implicitWidth: Math.max(dropdownMetrics.advanceWidth + 36, 80)
                    implicitHeight: 28

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

                    Rectangle {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        height: 1
                        color: dropdownDel.showError ? "#e53935" : "transparent"
                    }
                }
            }
        }
    }
}
