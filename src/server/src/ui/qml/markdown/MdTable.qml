pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Rectangle {
    id: root
    property var blockData: ({})
    property string fontFamily: ""
    property real fontSize: Theme.regularFontSize
    property real lineHeight: 1.0
    readonly property int columnCount: blockData.columnCount ?? 0
    readonly property var alignments: blockData.alignments ?? []
    readonly property real cellWidth: Math.max(40, width / Math.max(1, columnCount))
    readonly property color lineColor: Config.withAlpha(Theme.foreground, 0.1)

    width: parent?.width ?? 0
    implicitHeight: tableRows.contentHeight
    color: "transparent"
    border.width: 1
    border.color: root.lineColor
    radius: 6
    clip: true

    function textAlignment(column: int): int {
        const alignment = alignments[column] ?? 0;
        return alignment === 1 ? Text.AlignHCenter : alignment === 2 ? Text.AlignRight : Text.AlignLeft;
    }

    DocumentTableModel {
        id: metrics
        headers: root.blockData.headers ?? []
        rows: root.blockData.rows ?? []
        cellWidth: root.cellWidth
        lineHeight: root.lineHeight
        font: Qt.font({
            family: root.fontFamily || Theme.fontFamily,
            pointSize: root.fontSize
        })
    }

    DocumentLayout {
        id: tableRows
        width: root.width
        height: contentHeight
        model: metrics
        heightRole: "rowHeight"
        firstPartRole: "selectionOffset"
        embedded: true
        active: !root.DocumentScope.measuring
        viewport: root.DocumentScope.document?.flickable ?? null
        cacheBuffer: 64
        spacing: 0
        delegate: Rectangle {
            id: tableRow
            required property int index
            required property var cells
            required property bool header
            required property real rowHeight
            required property int selectionOffset
            width: root.width
            height: rowHeight
            color: header ? Config.withAlpha(Theme.foreground, 0.06) : "transparent"
            topLeftRadius: header ? root.radius : 0
            topRightRadius: header ? root.radius : 0

            Repeater {
                model: IndexModel {
                    count: tableRow.cells.length
                }
                Item {
                    id: cell
                    required property int index
                    x: index * root.cellWidth
                    width: root.cellWidth
                    height: tableRow.height
                    Rectangle {
                        anchors.right: parent.right
                        width: cell.index < root.columnCount - 1 ? 1 : 0
                        height: parent.height
                        color: root.lineColor
                    }
                    DocumentText {
                        anchors.fill: parent
                        anchors.margins: 8
                        textFormat: TextEdit.RichText
                        wrapMode: TextEdit.Wrap
                        font.family: root.fontFamily || Theme.fontFamily
                        fontSize: root.fontSize
                        lineHeight: root.lineHeight
                        font.bold: tableRow.header
                        horizontalAlignment: root.textAlignment(cell.index)
                        text: tableRow.cells[cell.index]?.html ?? ""
                        selectionPart: tableRow.selectionOffset + cell.index
                    }
                }
            }
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: tableRow.index < tableRows.count - 1 ? 1 : 0
                color: root.lineColor
            }
        }
    }
}
