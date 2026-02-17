import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root

    property var blockData: ({})
    property var mdModel: null
    property int blockIndex: -1

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
                    Layout.fillWidth: true
                    readOnly: true
                    selectByMouse: true
                    selectionColor: Theme.textSelectionBg
                    selectedTextColor: Theme.textSelectionFg
                    textFormat: TextEdit.RichText
                    wrapMode: TextEdit.Wrap
                    color: Theme.textMuted
                    font.pointSize: Theme.regularFontSize
                    font.italic: true
                    text: modelData ?? ""
                    onLinkActivated: link => { if (root.mdModel) root.mdModel.openLink(link) }
                }
            }
        }
    }
}
