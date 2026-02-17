import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root

    property var blockData: ({})
    property var mdModel: null
    property int blockIndex: -1
    property var selectionController: null

    readonly property var paragraphs: blockData.paragraphs ?? []

    width: parent?.width ?? 0
    implicitHeight: row.implicitHeight

    RowLayout {
        id: row
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.preferredWidth: 3
            Layout.fillHeight: true
            radius: 1.5
            color: Theme.divider
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 10
            spacing: 4

            Repeater {
                model: root.paragraphs

                TextEdit {
                    id: bqText
                    Layout.fillWidth: true
                    readOnly: true
                    selectionColor: Theme.textSelectionBg
                    selectedTextColor: Theme.textSelectionFg
                    textFormat: TextEdit.RichText
                    wrapMode: TextEdit.Wrap
                    color: Theme.textMuted
                    font.pointSize: Theme.regularFontSize
                    font.italic: true
                    text: modelData ?? ""

                    required property var modelData
                    required property int index

                    Component.onCompleted: if (root.selectionController) root.selectionController.registerSelectable(bqText, root.blockIndex * 10000 + index, true)
                    Component.onDestruction: if (root.selectionController) root.selectionController.unregisterSelectable(bqText)
                }
            }
        }
    }
}
