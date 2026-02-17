import QtQuick
import QtQuick.Layouts
import Vicinae

ColumnLayout {
    id: root

    property var blockData: ({})
    property var mdModel: null
    property int blockIndex: -1
    property var selectionController: null
    property bool ordered: false
    property int startNumber: blockData.startNumber ?? 1
    property int depth: 0

    width: parent?.width ?? 0
    spacing: 2

    Repeater {
        model: blockData.items ?? []

        ColumnLayout {
            id: itemDelegate
            Layout.fillWidth: true
            spacing: 2

            required property var modelData
            required property int index

            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: root.depth * 16
                spacing: 6

                Text {
                    Layout.alignment: Qt.AlignTop
                    Layout.topMargin: 2
                    text: root.ordered ? (root.startNumber + itemDelegate.index) + "." : "\u2022"
                    color: Theme.textMuted
                    font.pointSize: Theme.regularFontSize
                }

                TextEdit {
                    id: itemText
                    Layout.fillWidth: true
                    readOnly: true
                    selectionColor: Theme.textSelectionBg
                    selectedTextColor: Theme.textSelectionFg
                    textFormat: TextEdit.RichText
                    wrapMode: TextEdit.Wrap
                    color: Theme.foreground
                    font.pointSize: Theme.regularFontSize
                    text: itemDelegate.modelData.html ?? ""

                    Component.onCompleted: if (root.selectionController) root.selectionController.registerSelectable(itemText, root.blockIndex * 10000 + itemDelegate.index, true)
                    Component.onDestruction: if (root.selectionController) root.selectionController.unregisterSelectable(itemText)
                }
            }

            // Nested sub-lists — use Loader to break recursive instantiation
            Repeater {
                model: itemDelegate.modelData.children ?? []

                Loader {
                    Layout.fillWidth: true
                    source: "MdList.qml"
                    onLoaded: {
                        item.selectionController = root.selectionController
                        item.mdModel = root.mdModel
                        item.blockIndex = root.blockIndex
                        item.ordered = modelData.ordered ?? false
                        item.startNumber = modelData.startNumber ?? 1
                        item.depth = root.depth + 1
                        item.blockData = {items: modelData.items ?? []}
                    }
                }
            }
        }
    }
}
